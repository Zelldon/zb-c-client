/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "tngp.h"

/**
 * Connects to the server with the given parameter.
 * Uses the PORT makro for the connection port.
 *
 * @param host the host for the connection
 * @return the file descriptor of the socket
 */
int connectServer(const char* host) {
  //Maybe we use SOCK_SEQPACKET instead
  //SOCK_STREAM will end to TCP
  int filedescriptor = socket(AF_INET, SOCK_STREAM, 0);
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
  memcpy((char*) &serv_addr.sin_addr.s_addr, server->h_addr_list[0], sizeof (server->h_addr_list[0]));

  if (connect(filedescriptor, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
    return -1;
  }
  return filedescriptor;
}

void cleanUpMessage(struct Message* message) {
  if (message != NULL) {
    if (message->body != NULL) {
      if (message->body->body != NULL) {
        free(message->body->body);
      }
      free(message->body);
    }
    free(message);
  }
}

void _cleanUpTransportProtocol(struct TransportProtocol* transportProtocol) {
  if (transportProtocol != NULL) {
    if (transportProtocol->body != NULL) {
      free(transportProtocol->body);
    }
    free(transportProtocol);
  }
}

void _cleanUpTaskCreateMessage(struct TaskCreateMessage* taskCreateMessage) {
  if (taskCreateMessage != NULL) {
    if (taskCreateMessage->taskType == NULL) {
      free(taskCreateMessage->taskType);
    }
    if (taskCreateMessage->payload == NULL) {
      free(taskCreateMessage->payload);
    }
    free(taskCreateMessage);
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
struct Message* _createMessage(struct TransportProtocol* transportProtocol, int len) {
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

  //copy transport protocol message into body
  message->body = malloc(sizeof (struct TransportProtocol));
  if (message->body == NULL) {
    return message;
  }
  memcpy(message->body, transportProtocol, len);

  int transportProtocolBodyLen = transportProtocol->bodyLen;
  message->body->body = calloc(transportProtocolBodyLen, sizeof (char));
  if (message->body->body == NULL) {
    return message;
  }
  memcpy(message->body->body, transportProtocol->body, transportProtocolBodyLen);
  return message;
}

/**
 * Sends the given message to the connected server with help of the given file descriptor.
 *
 * @param fileDescriptor the file descriptor for the connected server
 * @param message the message struct which contains the header data and message content
 * @param len the length of the hole message
 * @return the status of the send
 */
int _sendMessage(int fileDescriptor, char* message, int len) {
  int writtenBytes = 0;
  while (writtenBytes < len) {

    if (writtenBytes != 0 && lseek(fileDescriptor, writtenBytes, SEEK_SET) < 0) {
      puts("Seek failed!");
      return -1;
    }

    int n = write(fileDescriptor, message, len);
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
int _allignedSize(int size) {
  return (size + (8 - 1)) & ~(8 - 1);
}

char* _createBufferFromMessage(struct Message* message, int alignedSize) {
  //create buffer
  char* buffer = calloc(alignedSize, sizeof (char));
  if (buffer == NULL) {
    return buffer;
  }

  //copy values
  if (alignedSize >= MSG_HEADER_LEN) {
    //copy message header
    memcpy(buffer, (char*) message, MSG_HEADER_LEN);
    if (alignedSize >= MSG_HEADER_LEN + TRANS_HEADER_LEN) {
      //copy transport protocol message header
      memcpy(&buffer[MSG_HEADER_LEN], message->body, TRANS_HEADER_LEN);
      if (message->body->templateId == CREATE_TASK_REQUEST
              && alignedSize >= MSG_HEADER_LEN + TRANS_HEADER_LEN + message->body->bodyLen) {
        //copy task create message
        char b[message->body->bodyLen];
        memcpy(b, message->body->body, message->body->bodyLen);
        memcpy(&buffer[MSG_HEADER_LEN + TRANS_HEADER_LEN], message->body->body, message->body->bodyLen);

      }  else if (message->body->templateId == POLL_AND_LOCK_REQUEST
              && alignedSize >= MSG_HEADER_LEN + TRANS_HEADER_LEN + message->body->bodyLen) {
        memcpy(&buffer[MSG_HEADER_LEN + TRANS_HEADER_LEN], message->body->body, message->body->bodyLen);

      }
    }
  }

  return buffer;
}

int sendMessage(int fileDescriptor, struct TransportProtocol* transportProtocol, int len) {

  struct Message* message = _createMessage(transportProtocol, len);
  if (message == NULL) {
    return -1;
  }

  int bytes = MSG_HEADER_LEN + len;
  //we need buffer aligned to 8 divid
  int alignedSize = _allignedSize(bytes);
  char* buffer = _createBufferFromMessage(message, alignedSize);
  if (buffer == NULL) {
    cleanUpMessage(message);
    return -1;
  }
  //send message
  _sendMessage(fileDescriptor, buffer, alignedSize);

  //clean up
  free(buffer);
  cleanUpMessage(message);
  return 0;
}

void _writeTaskCreateMessageToBody(struct TaskCreateMessage* taskCreateMsg, char* body, int bodySize) {
  int offset = 0;
  if (offset + TASK_CREATE_HEADER_TYPE_LEN < bodySize) {
    memcpy(body, &taskCreateMsg->taskType->length, TASK_CREATE_HEADER_TYPE_LEN);
    offset += TASK_CREATE_HEADER_TYPE_LEN;
    if (offset + taskCreateMsg->taskType->length < bodySize) {
      memcpy(&body[offset], taskCreateMsg->taskType->data, taskCreateMsg->taskType->length);
      offset += taskCreateMsg->taskType->length;
      if (offset + TASK_CREATE_HEADER_PAYLOAD_LEN <= bodySize) {
        memcpy(&body[offset], &taskCreateMsg->payload->length, TASK_CREATE_HEADER_PAYLOAD_LEN);
        offset += TASK_CREATE_HEADER_TYPE_LEN;
        if (offset + taskCreateMsg->payload->length < bodySize) {
          memcpy(&body[offset], taskCreateMsg->payload->data, taskCreateMsg->payload->length);
        }
      }
    }
  }
}

void _writePollAndLockTaskMessageToBody(struct PollAndLockTaskMessage* pollAndLockTaskMessage, char* body, int bodySize) {
  int offset = 0;
  if (offset + POLL_AND_LOCK_HEADER_LEN < bodySize) {

    offset = 0;
    memcpy(body, &pollAndLockTaskMessage->consumerId, sizeof(short));
    offset += sizeof(short);

    memcpy(&body[offset], &pollAndLockTaskMessage->lockTime, sizeof(long));
    offset += sizeof(long);

    memcpy(&body[offset], &pollAndLockTaskMessage->maxTasks, sizeof(short));
    offset += sizeof(short);

    if (offset + POLL_AND_LOCK_TYPE_LEN < bodySize) {
      memcpy(&body[offset], pollAndLockTaskMessage->taskType, POLL_AND_LOCK_TYPE_LEN);
      offset += POLL_AND_LOCK_TYPE_LEN;
      if (offset + pollAndLockTaskMessage->taskType->length <= bodySize) {
        memcpy(&body[offset], pollAndLockTaskMessage->taskType->data, pollAndLockTaskMessage->taskType->length);
      }
    }
  }
}

int _initTransportProtocolBody(struct TransportProtocol* transportProtocol, void* message) {

  int bodySize = transportProtocol->bodyLen;
  transportProtocol->body = calloc(bodySize, sizeof (char));
  if (transportProtocol->body == NULL) {
    _cleanUpTransportProtocol(transportProtocol);
    return -1;
  }

  if (transportProtocol->templateId == CREATE_TASK_REQUEST) {
    _writeTaskCreateMessageToBody((struct TaskCreateMessage*) message, transportProtocol->body, bodySize);
  } else if (transportProtocol->templateId == POLL_AND_LOCK_REQUEST) {
    _writePollAndLockTaskMessageToBody((struct PollAndLockTaskMessage*) message, transportProtocol->body, bodySize);
  }

  return 0;
}

struct TransportProtocol* _createTransportProtocol(short templateId) {
  struct TransportProtocol* transportProtocol = malloc(sizeof (struct TransportProtocol));
  if (transportProtocol == NULL) {
    return NULL;
  }
  //generated by the client
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

struct VariableData* _createVariableData(const char* data) {
  struct VariableData* variableData = malloc(sizeof (struct VariableData));
  if (variableData == NULL) {
    return NULL;
  }

  if (data == NULL) {
    variableData->data = NULL;
    variableData->length = 0;
  } else {
    variableData->length = strlen(data);
    variableData->data = calloc(variableData->length, sizeof (char));
    if (variableData->data == NULL) {
      return NULL;
    }
    memcpy(variableData->data, data, variableData->length);
  }
  return variableData;
}

int createTask(int fileDescriptor, const char* topic) {
  struct TransportProtocol* transportProtocol = _createTransportProtocol(CREATE_TASK_REQUEST);
  if (transportProtocol == NULL) {
    return -1;
  }
  //init create message
  struct TaskCreateMessage* taskCreateMsg = malloc(sizeof (struct TaskCreateMessage));
  if (taskCreateMsg == NULL) {
    return -1;
  }
  taskCreateMsg->taskType = _createVariableData(topic);
  taskCreateMsg->payload = _createVariableData(NULL); //not supported yet

  transportProtocol->bodyLen = TASK_CREATE_HEADER_LEN + taskCreateMsg->taskType->length + taskCreateMsg->payload->length;

  if (_initTransportProtocolBody(transportProtocol, taskCreateMsg) < 0) {
    _cleanUpTaskCreateMessage(taskCreateMsg);
    _cleanUpTransportProtocol(transportProtocol);
    return -1;
  }

  //send message
  int result = sendMessage(fileDescriptor, transportProtocol, TRANS_HEADER_LEN + transportProtocol->bodyLen);

  //clean up
  _cleanUpTaskCreateMessage(taskCreateMsg);
  _cleanUpTransportProtocol(transportProtocol);
  return result;
}

struct Message* readCreateTaskServerAck(int fileDescriptor) {
  const int ackSize = 48;
  char* buffer = calloc(ackSize, sizeof (char));
  if (buffer == NULL) {
    return NULL;
  }
  int n = read(fileDescriptor, buffer, ackSize);


  struct Message* serverAck = malloc(sizeof (struct Message));
  if (serverAck == NULL) {
    return NULL;
  }

  if (n > MSG_HEADER_LEN) {
    memcpy(serverAck, buffer, MSG_HEADER_LEN);
    if (n > MSG_HEADER_LEN + TRANS_HEADER_LEN) {

      serverAck->body = malloc(sizeof (struct TransportProtocol));
      if (serverAck->body == NULL) {
        cleanUpMessage(serverAck);
        return NULL;
      }
      memcpy(serverAck->body, &buffer[MSG_HEADER_LEN], TRANS_HEADER_LEN);

      if (n >= MSG_HEADER_LEN + TRANS_HEADER_LEN + SERVER_ACK_LEN) {
        serverAck->body->body = malloc(sizeof(struct SingleTaskServerAckMessage));
        if (serverAck->body->body == NULL) {
          cleanUpMessage(serverAck);
          return NULL;
        }
        memcpy(serverAck->body->body, &buffer[MSG_HEADER_LEN + TRANS_HEADER_LEN], SERVER_ACK_LEN);
        serverAck->body->bodyLen = SERVER_ACK_LEN;
      }
    }
  }
  return serverAck;
}


int pollAndLockTask(int fileDescriptor, const char* taskTopic) {
  struct TransportProtocol* transportProtocol = _createTransportProtocol(POLL_AND_LOCK_REQUEST);
  if (transportProtocol == NULL) {
    return -1;
  }
  transportProtocol->blockLength = POLL_AND_LOCK_HEADER_LEN;


  struct PollAndLockTaskMessage* pollAndlockTaskMessage = malloc(sizeof (struct PollAndLockTaskMessage));
  if (pollAndlockTaskMessage == NULL) {
    _cleanUpTransportProtocol(transportProtocol);
    return -1;
  }

  pollAndlockTaskMessage->consumerId = 0;
  pollAndlockTaskMessage->lockTime = 15 * 60 * 1000;
  pollAndlockTaskMessage->maxTasks = 1;
  pollAndlockTaskMessage->taskType = _createVariableData(taskTopic);
  transportProtocol->bodyLen = POLL_AND_LOCK_HEADER_LEN + POLL_AND_LOCK_TYPE_LEN + pollAndlockTaskMessage->taskType->length;


  if (_initTransportProtocolBody(transportProtocol, pollAndlockTaskMessage) < 0) {
    _cleanUpPollAndLockTaskMessage(pollAndlockTaskMessage);
    _cleanUpTransportProtocol(transportProtocol);
    return -1;
  }

  //send message
  int result = sendMessage(fileDescriptor, transportProtocol, TRANS_HEADER_LEN + transportProtocol->bodyLen);

  //clean up
  _cleanUpPollAndLockTaskMessage(pollAndlockTaskMessage);
  _cleanUpTransportProtocol(transportProtocol);
  return result;

}


struct Message* readPollAndLockServerAck(int fileDescriptor) {
  const int ackSize = 256;
  char* buffer = calloc(ackSize, sizeof (char));
  if (buffer == NULL) {
    return NULL;
  }
  int n = read(fileDescriptor, buffer, ackSize);


  struct Message* serverAck = malloc(sizeof (struct Message));
  if (serverAck == NULL) {
    return NULL;
  }

  if (n > MSG_HEADER_LEN) {
    memcpy(serverAck, buffer, MSG_HEADER_LEN);
    if (n > MSG_HEADER_LEN + TRANS_HEADER_LEN) {

      serverAck->body = malloc(sizeof (struct TransportProtocol));
      if (serverAck->body == NULL) {
        cleanUpMessage(serverAck);
        return NULL;
      }
      memcpy(serverAck->body, &buffer[MSG_HEADER_LEN], TRANS_HEADER_LEN);

      if (n >= MSG_HEADER_LEN + TRANS_HEADER_LEN + LOCKED_TASK_BATCH_HEADER) {
        serverAck->body->body = malloc(sizeof(struct LockedTaskBatchMessage));
        if (serverAck->body->body == NULL) {
          cleanUpMessage(serverAck);
          return NULL;
        }

        memcpy(serverAck->body->body, &buffer[MSG_HEADER_LEN + TRANS_HEADER_LEN], LOCKED_TASK_BATCH_HEADER);
        serverAck->body->bodyLen = LOCKED_TASK_BATCH_HEADER;
      }
    }
  }
  return serverAck;
}
