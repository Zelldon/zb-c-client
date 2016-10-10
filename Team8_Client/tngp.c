/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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

void cleanUpMessage(struct Message* message) {
  if (message != NULL) {
    if (message->body != NULL) {
      free(message->body);
    }
    free(message);
  }
}

void _cleanUpTaskCreateMessage(struct TaskCreateMessage* taskCreateMessage) {
  if (taskCreateMessage != NULL) {
    cleanUpMessage(taskCreateMessage->head);
    if (taskCreateMessage->taskType != NULL) {
      free(taskCreateMessage->taskType);
    }
    if (taskCreateMessage->payload != NULL) {
      free(taskCreateMessage->payload);
    }
    free(taskCreateMessage);
  }
}

void cleanUpSingleTaskServerAckMessage(struct SingleTaskServerAckMessage* ack) {
  if (ack != NULL) {
    cleanUpMessage(ack->head);
    free(ack);
  }
}

void _cleanUpPollAndLockTaskMessage(struct PollAndLockTaskMessage* pollAndLockTaskMessage) {
  if (pollAndLockTaskMessage != NULL) {
    if (pollAndLockTaskMessage->taskType != NULL) {
      free(pollAndLockTaskMessage->taskType);
    }
    free(pollAndLockTaskMessage);
  }
}

/**
 * Creates a message structure, with the given message.
 *
 * @param transportProtocol the transport protocol message, which should be a composite of the message struct.
 * @param len the length of the message
 * @return a pointer of the created message struct
 */
struct Message* _createMessage(struct TransportProtocol* transportProtocol, int32_t len) {
  // init message
  struct Message* message = malloc(sizeof (struct Message));
  if (message == NULL) {
    return message;
  }

  message->len = len; // htonl(len);
  message->version = 1;
  message->flags = 2;
  message->type = 0; //htons(4);
  message->streamId = 1; //htonl(1);

  //copy transport protocol message int32_to body
  message->body = malloc(sizeof (struct TransportProtocol));
  if (message->body == NULL) {
    return message;
  }
  memcpy(message->body, transportProtocol, len);
  return message;
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

//uint8_t* _createBufferFromMessage(struct Message* message, int32_t alignedSize) {
//  //create buffer
//  uint8_t* buffer = calloc(alignedSize, sizeof (uint8_t));
//  if (buffer == NULL) {
//    return buffer;
//  }
//
//  //copy values
//  if (alignedSize >= MSG_HEADER_LEN) {
//    //copy message header
//    memcpy(buffer, (uint8_t*) message, MSG_HEADER_LEN);
//    if (alignedSize >= MSG_HEADER_LEN + TRANS_HEADER_LEN) {
//      //copy transport protocol message header
//      memcpy(&buffer[MSG_HEADER_LEN], message->body, TRANS_HEADER_LEN);
//      if (message->body->templateId == CREATE_TASK_REQUEST
//              && alignedSize >= MSG_HEADER_LEN + TRANS_HEADER_LEN + message->body->bodyLen) {
//        //copy task create message
//        uint8_t b[message->body->bodyLen];
//        memcpy(b, message->body->body, message->body->bodyLen);
//        memcpy(&buffer[MSG_HEADER_LEN + TRANS_HEADER_LEN], message->body->body, message->body->bodyLen);
//
//      } else if (message->body->templateId == POLL_AND_LOCK_REQUEST
//              && alignedSize >= MSG_HEADER_LEN + TRANS_HEADER_LEN + message->body->bodyLen) {
//        memcpy(&buffer[MSG_HEADER_LEN + TRANS_HEADER_LEN], message->body->body, message->body->bodyLen);
//
//      }
//    }
//  }
//
//  return buffer;
//}

//int32_t sendMessage(int32_t fileDescriptor, struct TransportProtocol* transportProtocol, int32_t len) {
//
//  struct Message* message = _createMessage(transportProtocol, len);
//  if (message == NULL) {
//    return -1;
//  }
//
//  int32_t bytes = MSG_HEADER_LEN + len;
//  //we need buffer aligned to 8 divid
//  int32_t alignedSize = _allignedSize(bytes);
//  uint8_t* buffer = _createBufferFromMessage(message, alignedSize);
//  if (buffer == NULL) {
//    cleanUpMessage(message);
//    return -1;
//  }
//  //send message
//  _sendMessage(fileDescriptor, buffer, alignedSize);
//
//  //clean up
//  free(buffer);
//  cleanUpMessage(message);
//  return 0;
//}

struct TransportProtocol* _createTransportProtocol(short templateId) {
  struct TransportProtocol* transportProtocol = malloc(sizeof (struct TransportProtocol));
  if (transportProtocol == NULL) {
    return NULL;
  }
  //generated by the client
  transportProtocol->protocolId = 0;
  transportProtocol->connectionId = 2562132L;
  transportProtocol->requestId = 2551312L;

  //init protocol
  transportProtocol->blockLength = 0;
  transportProtocol->templateId = templateId;
  transportProtocol->schemaId = 1;
  transportProtocol->version = 1;
  transportProtocol->resourceId = 0;
  transportProtocol->shardId = -2; //not supported yet
  return transportProtocol;
}

struct VariableData* _createVariableData(const uint8_t* data) {
  struct VariableData* variableData = malloc(sizeof (struct VariableData));
  if (variableData == NULL) {
    return NULL;
  }

  if (data == NULL) {
    variableData->data = NULL;
    variableData->length = 0;
  } else {
    variableData->length = strlen(data);
    variableData->data = calloc(variableData->length, sizeof (uint8_t));
    if (variableData->data == NULL) {
      return NULL;
    }
    memcpy(variableData->data, data, variableData->length);
  }
  return variableData;
}

struct TaskCreateMessage* _createTaskCreateMessage(const uint8_t* topic) {
  struct TaskCreateMessage* taskCreateMsg = malloc(sizeof (struct TaskCreateMessage));
  if (taskCreateMsg == NULL) {
    return NULL;
  }

  struct TransportProtocol* transportProtocol = _createTransportProtocol(CREATE_TASK_REQUEST);
  if (transportProtocol == NULL) {
    return NULL;
  }

  taskCreateMsg->taskType = _createVariableData(topic);
  taskCreateMsg->payload = _createVariableData(NULL); //not supported yet

  transportProtocol->bodyLen = TASK_CREATE_HEADER_LEN
                             + taskCreateMsg->taskType->length
                             + taskCreateMsg->payload->length;

  int32_t len = TRANS_HEADER_LEN + transportProtocol->bodyLen;
  taskCreateMsg->head = _createMessage(transportProtocol, len);
  if (taskCreateMsg->head == NULL) {
    return NULL;
  }
  return taskCreateMsg;
}

struct SingleTaskServerAckMessage* createTask(int32_t fileDescriptor, const uint8_t* topic) {
  //init create message
  struct TaskCreateMessage* taskCreateMsg = _createTaskCreateMessage(topic);
  if (taskCreateMsg == NULL) {
    return NULL;
  }

  //we need buffer aligned to 8 divid
  int32_t bytes = MSG_HEADER_LEN + taskCreateMsg->head->len;
  int32_t alignedSize = _allignedSize(bytes);

  uint8_t* buffer = calloc(alignedSize, sizeof (uint8_t));
  if (buffer == NULL) {
    _cleanUpTaskCreateMessage(taskCreateMsg);
    return NULL;
  }
  uint8_t* nextFreeBuf = writeTaskCreateMessage(buffer, taskCreateMsg);
  if (nextFreeBuf == NULL) {
    _cleanUpTaskCreateMessage(taskCreateMsg);
    return NULL;
  }
  //send message
  int32_t result = _sendBytes(fileDescriptor, buffer, alignedSize);
  struct SingleTaskServerAckMessage* ack = NULL;
  if (result > 0) {
    ack = readCreateTaskServerAck(fileDescriptor);
  }

  //clean up buffer and task message
  _cleanUpTaskCreateMessage(taskCreateMsg);
  free(buffer);
  return ack;
}

struct SingleTaskServerAckMessage* readCreateTaskServerAck(int32_t fileDescriptor) {
  //read acknowledgment
  const int32_t ackSize = 56;
  uint8_t* buffer = _readBytes(fileDescriptor, ackSize);
  if (buffer == NULL) {
    return NULL;
  }
  //allocate space for acknowledge message
  struct SingleTaskServerAckMessage* serverAck = malloc(sizeof (struct SingleTaskServerAckMessage));
  if (serverAck == NULL) {
    return NULL;
  }
  // read from buff into single task server acknowledge message
  uint8_t* nextFreeBuf = readSingleTaskServerAckMessage(buffer, serverAck);
  if (nextFreeBuf == NULL) {
    return NULL;
  }
  return serverAck;
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
