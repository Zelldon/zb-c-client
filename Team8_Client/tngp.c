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
 * @param msg the message which should be a composite of the message struct.
 * @param len the length of the message
 * @return a pointer of the created message struct
 */
struct Message* _createMessage(char* msg, int len) {
  // init message
  struct Message* message = malloc(sizeof(struct Message));
  if (message == NULL) {
    exit(-1);
  }

  message->len = len;// htonl(len);
  message->version = 1;
  message->flags = 2;
  message->type = 0;//htons(4);
  message->streamId = 1;//htonl(1);

  message->msg = malloc(len);
  if (message == NULL) {
    exit(-1);
  }

  memcpy(message->msg, msg, len);

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
/**
 * Sends the message to the connected server, with help of the file descriptor.
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @param msg the message which should be send
 * @return the status of the send
 */
int sendMessage(int fileDescriptor, char* msg) {

  int len = strlen(msg);
  if (len > 256) {
    return -1;
  }

  struct Message* message = _createMessage(msg, len);
  int bytes =  header_len + len;
  //we need buffer aligned to 8 divid
  int allignedSize = _allignedSize(bytes);

  //create buffer and copy values
  char* buffer = malloc(allignedSize);
  memcpy(buffer, (char*) message, bytes);
  memcpy(&buffer[header_len], message->msg, len);

  //send message
  _sendMessage(fileDescriptor, buffer, bytes);

  //clean up
  free(buffer);
  free(message->msg);
  free(message);
  return 0;
}