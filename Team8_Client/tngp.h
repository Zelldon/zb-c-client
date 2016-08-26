/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   tngp.h
 * Author: Christopher Zell <christopher.zell@camunda.com>
 *
 * Created on August 24, 2016, 3:51 PM
 */

#ifndef TNGP_H
#define TNGP_H



#ifdef __cplusplus
extern "C" {
#endif

//network libs
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//util
#include<string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define CREATE_TASK_REQUEST 0
#define POLL_AND_LOCK_REQUEST 1

#define PORT 51015

#define MSG_HEADER_LEN 12

struct Message {
  //head
  int len;
  char version;
  char flags;
  short type;
  int streamId;
  //body
  struct TransportProtocol* body;
};

#define TRANS_HEADER_LEN 28

struct TransportProtocol {
  long connectionId;
  long requestId;

  //header
  short blockLength;
  short templateId;
  short schemaId;
  short version;
  short resourceId;
  short shardId;
  //body
  long bodyLen;
  char* body;
};


struct VariableData {
  short length;
  char* data;
};

#define TASK_CREATE_HEADER_LEN TASK_CREATE_HEADER_TYPE_LEN + TASK_CREATE_HEADER_PAYLOAD_LEN
#define TASK_CREATE_HEADER_TYPE_LEN 2
#define TASK_CREATE_HEADER_PAYLOAD_LEN 2

struct TaskCreateMessage {
  //task type
  struct VariableData* taskType;
  //payload
  struct VariableData* payload;
};

#define SERVER_ACK_LEN 8

struct SingleTaskServerAckMessage {
  long taskId;
};


#define POLL_AND_LOCK_HEADER_LEN 12
#define POLL_AND_LOCK_TYPE_LEN 2
struct PollAndLockTaskMessage {
  short consumerId;
  long lockTime;
  short maxTasks;

  //task type
  struct VariableData* taskType;
};

/**
 * Connects to the server with the given parameter.
 * Uses the PORT makro for the connection port.
 *
 * @param host the host for the connection
 * @return the file descriptor of the socket
 */
int connectServer(const char* host);

/**
 * Sends the transport protocol message to the connected server, with help of the file descriptor.
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @param transportProtocol the transportProtocol message which should be send
 * @param len the length of the hole message
 * @return the status of the send
 */
int sendMessage(int fileDescriptor, struct TransportProtocol* transportProtocol, int len);


/**
 * Reads the acknowledgment of the server that a task was created,
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @return the message which was read and send by the server. The server contains the header and single task acknowledgment.
 *         NULL if something goes wrong.
 */
struct Message* readServerAck(int fileDescriptor);

/**
 * Uses the given file descriptor to send a create task message to the server.
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @param topic the topic of the task
 * @return the status of the send
 */
int createTask(int fileDescriptor, const char* topic);


int pollAndLockTask(int fileDescriptor, const char* topic);

#ifdef __cplusplus
}
#endif

#endif /* TNGP_H */

