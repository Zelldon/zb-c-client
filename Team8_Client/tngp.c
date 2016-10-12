/*
 * Copyright 2016 Christopher Zell <christopher.zell@camunda.com>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tngp.h"

/**
 * Reads a bunch of bytes from the socket which is identified by the given file descriptor.
 *
 * @param fileDescriptor the file descriptor which identified the socket
 * @param size the size of the array which should be read
 * @return the pointer of the read bytes
 */
uint8_t* _readBytes(int32_t fileDescriptor, uint32_t size) {
  uint8_t* buffer = calloc(size, sizeof (uint8_t));
  if (buffer == NULL) {
    return NULL;
  }

  int32_t n = read(fileDescriptor, buffer, size);
  if (n > 0) {
    return buffer;
  } else {
    return NULL;
  }
}

/**
 * Sends the given message to the connected server with help of the given file descriptor.
 *
 * @param fileDescriptor the file descriptor for the connected server
 * @param message the message struct which contains the header data and message content
 * @param len the length of the hole message
 * @return the status of the send
 */
int32_t _sendBytes(int32_t fileDescriptor, uint8_t* message, int32_t len) {
  int32_t writtenBytes = 0;
  while (writtenBytes < len) {

    if (writtenBytes != 0 && lseek(fileDescriptor, writtenBytes, SEEK_SET) < 0) {
      puts("Seek failed!");
      return -1;
    }

    int32_t n = write(fileDescriptor, message, len);
    if (n <= 0) {
      puts("Write failed!");
      return -1;
    }

    writtenBytes += n;
    printf("\nWrite %d bytes\n", n);
  }
}

/**
 * Returns next aligned size. The returned size is completely divisible with 8.
 *
 * @param size the size which should be aligned
 * @return the aligned size
 */
int32_t _allignedSize(int32_t size) {
  return (size + (8 - 1)) & ~(8 - 1);
}


/**
 * Reads the acknowledgment of the server that a task was created,
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @return the message which was read and send by the server. The server contains the header and single task acknowledgment.
 *         NULL if something goes wrong.
 */
struct SingleTaskServerAckMessage* _readCreateTaskServerAck(int32_t fileDescriptor) {
  //read acknowledgment
  const int32_t ackSize = 56;
  uint8_t* buffer = _readBytes(fileDescriptor, ackSize);
  if (buffer == NULL) {
    return NULL;
  }
  //allocate space for acknowledge message
  struct SingleTaskServerAckMessage* serverAck = malloc(sizeof (struct SingleTaskServerAckMessage));
  if (serverAck == NULL) {
    free(buffer);
    return NULL;
  }
  // read from buff into single task server acknowledge message
  readSingleTaskServerAckMessage(buffer, serverAck);
  free(buffer);
  return serverAck;
}

/**
 * Connects to the server with the given parameter.
 * Uses the PORT makro for the connection port.
 *
 * @param host the host for the connection
 * @return the file descriptor of the socket
 */
int32_t connectServer(const uint8_t* host) {
  //Maybe we use SOCK_SEQPACKET instead
  //SOCK_STREAM will end to TCP
  int32_t filedescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (filedescriptor < 0) {
    return -1;
  }

  //get host
  struct hostent *server;
  server = gethostbyname(host);

  //init  server addr
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_family = AF_INET;
  memcpy((uint8_t*) &serv_addr.sin_addr.s_addr, server->h_addr_list[0], sizeof (server->h_addr_list[0]));

  if (connect(filedescriptor, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
    return -1;
  }
  return filedescriptor;
}

struct SingleTaskServerAckMessage* createTask(int32_t fileDescriptor, const uint8_t* topic) {
  //init create message
  struct TaskCreateMessage* taskCreateMsg = createTaskCreateMessage(topic);
  if (taskCreateMsg == NULL) {
    return NULL;
  }

  //we need buffer aligned to 8 divid
  int32_t bytes = MSG_HEADER_LEN + taskCreateMsg->head->len;
  int32_t alignedSize = _allignedSize(bytes);

  uint8_t* buffer = calloc(alignedSize, sizeof (uint8_t));
  if (buffer == NULL) {
    freeTaskCreateMessage(taskCreateMsg);
    return NULL;
  }
  writeTaskCreateMessage(buffer, taskCreateMsg);

  //send message
  int32_t result = _sendBytes(fileDescriptor, buffer, alignedSize);
  struct SingleTaskServerAckMessage* ack = NULL;
  if (result > 0) {
    ack = _readCreateTaskServerAck(fileDescriptor);
  }

  //clean up buffer and task message
  freeTaskCreateMessage(taskCreateMsg);
  free(buffer);
  return ack;
}


//
//int32_t pollAndLockTask(int32_t fileDescriptor, const uint8_t* taskTopic) {
//  struct TransportProtocol* transportProtocol = _createTransportProtocol(POLL_AND_LOCK_REQUEST);
//  if (transportProtocol == NULL) {
//    return -1;
//  }
//  transportProtocol->blockLength = POLL_AND_LOCK_HEADER_LEN;
//
//
//  struct PollAndLockTaskMessage* pollAndlockTaskMessage = malloc(sizeof (struct PollAndLockTaskMessage));
//  if (pollAndlockTaskMessage == NULL) {
//    _cleanUpTransportProtocol(transportProtocol);
//    return -1;
//  }
//
//  pollAndlockTaskMessage->consumerId = 0;
//  pollAndlockTaskMessage->lockTime = 15 * 60 * 1000;
//  pollAndlockTaskMessage->maxTasks = 1;
//  pollAndlockTaskMessage->taskType = _createVariableData(taskTopic);
//  transportProtocol->bodyLen = POLL_AND_LOCK_HEADER_LEN + POLL_AND_LOCK_TYPE_LEN + pollAndlockTaskMessage->taskType->length;
//
//
//  if (_initTransportProtocolBody(transportProtocol, pollAndlockTaskMessage) < 0) {
//    _cleanUpPollAndLockTaskMessage(pollAndlockTaskMessage);
//    _cleanUpTransportProtocol(transportProtocol);
//    return -1;
//  }
//
//  //send message
//  int32_t result = sendMessage(fileDescriptor, transportProtocol, TRANS_HEADER_LEN + transportProtocol->bodyLen);
//
//  //clean up
//  _cleanUpPollAndLockTaskMessage(pollAndlockTaskMessage);
//  _cleanUpTransportProtocol(transportProtocol);
//  return result;
//
//}
//
//struct Message* readPollAndLockServerAck(int32_t fileDescriptor) {
//  const int32_t ackSize = 256;
//  uint8_t* buffer = calloc(ackSize, sizeof (uint8_t));
//  if (buffer == NULL) {
//    return NULL;
//  }
//  int32_t n = read(fileDescriptor, buffer, ackSize);
//
//
//  struct Message* serverAck = malloc(sizeof (struct Message));
//  if (serverAck == NULL) {
//    return NULL;
//  }
//
//  if (n > MSG_HEADER_LEN) {
//    memcpy(serverAck, buffer, MSG_HEADER_LEN);
//    if (n > MSG_HEADER_LEN + TRANS_HEADER_LEN) {
//
//      serverAck->body = malloc(sizeof (struct TransportProtocol));
//      if (serverAck->body == NULL) {
//        cleanUpMessage(serverAck);
//        return NULL;
//      }
//      memcpy(serverAck->body, &buffer[MSG_HEADER_LEN], TRANS_HEADER_LEN);
//
//      if (n >= MSG_HEADER_LEN + TRANS_HEADER_LEN + LOCKED_TASK_BATCH_HEADER) {
//        serverAck->body->body = malloc(sizeof (struct LockedTaskBatchMessage));
//        if (serverAck->body->body == NULL) {
//          cleanUpMessage(serverAck);
//          return NULL;
//        }
//
//        memcpy(serverAck->body->body, &buffer[MSG_HEADER_LEN + TRANS_HEADER_LEN], LOCKED_TASK_BATCH_HEADER);
//        serverAck->body->bodyLen = LOCKED_TASK_BATCH_HEADER;
//      }
//    }
//  }
//  return serverAck;
//}
