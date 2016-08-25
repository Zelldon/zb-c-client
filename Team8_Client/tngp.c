/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>

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

  if(connect(filedescriptor, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {
    return -1;
  }
  return filedescriptor;
}

/**
 * Creates a message structure, with the given message.
 *
 * @param transportProtocol the transport protocol message, which should be a composite of the message struct.
 * @param len the length of the message
 * @return a pointer of the created message struct
 */
struct Message* _createMessage(struct TransportProtocol* transportPortocol, int len) {
  // init message
  struct Message* message = malloc(sizeof(struct Message));
  if (message == NULL) {
    return message;
  }

  message->len = len;// htonl(len);
  message->version = 1;
  message->flags = 2;
  message->type = 0;//htons(4);
  message->streamId = 1;//htonl(1);

  //copy transport protocol message into body
  message->body = malloc(sizeof(struct TransportProtocol));
  if (message->body == NULL) {
    return message;
  }
  memcpy(message->body, transportPortocol, len);

  message->body->body = malloc(sizeof(struct TaskCreateMessage));
  if (message->body->body == NULL) {
    return message;
  }
  memcpy(message->body->body, transportPortocol->body, TASK_CREATE_HEADER_LEN);

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
    printf("Write %d bytes\n", n);
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

int sendMessage(int fileDescriptor, struct TransportProtocol* transportProtocol, int len) {

  struct Message* message = _createMessage(transportProtocol, len);
  if (message == NULL) {
    return -1;
  }

  int bytes =  MSG_HEADER_LEN + len;
  //we need buffer aligned to 8 divid
  int allignedSize = _allignedSize(bytes);

  //create buffer and copy values
  char* buffer = calloc(allignedSize, sizeof(char));
  memcpy(buffer, (char*) message, MSG_HEADER_LEN);
  memcpy(&buffer[MSG_HEADER_LEN], message->body, TRANS_HEADER_LEN);
  memcpy(&buffer[MSG_HEADER_LEN + TRANS_HEADER_LEN], message->body->body, TASK_CREATE_HEADER_LEN);

  //send message
  _sendMessage(fileDescriptor, buffer, allignedSize);

  //clean up
  free(buffer);
  free(message->body->body);
  free(message->body);
  free(message);
  return 0;
}


int createTask(int fileDescriptor) {
  struct TransportProtocol* transportProtocol = malloc(sizeof(struct TransportProtocol));
  if (transportProtocol == NULL) {
    return -1;
  }
  //generated by the client
  transportProtocol->connectionId = 2562132L;
  transportProtocol->requestId = 2551312L;

  //init protocol
  transportProtocol->blockLength = 0;
  transportProtocol->templateId = 0;
  transportProtocol->schemaId = 1;
  transportProtocol->version = 1;
  transportProtocol->resourceId = 0;
  transportProtocol->shardId = -2;

  //init create message
  struct TaskCreateMessage taskCreateMsg;
  taskCreateMsg.taskTypeLength = 1;//?
  taskCreateMsg.taskTypeVarData = 2;//?
  taskCreateMsg.payloadLen = 1;
  taskCreateMsg.payloadVarData = 4;

  transportProtocol->body = malloc(sizeof(struct TaskCreateMessage));
  if (transportProtocol->body == NULL) {
    return -1;
  }
  memcpy(transportProtocol->body, &taskCreateMsg, TASK_CREATE_HEADER_LEN);

  //send message
  int result = sendMessage(fileDescriptor, transportProtocol, TRANS_HEADER_LEN + TASK_CREATE_HEADER_LEN);

  //clean up
  free(transportProtocol->body);
  free(transportProtocol);
  return result;
}