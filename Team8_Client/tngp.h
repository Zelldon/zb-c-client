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
#include "protocol.h"
//util
#include<string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


#define PORT 51015

/**
 * Connects to the server with the given parameter.
 * Uses the PORT makro for the connection port.
 *
 * @param host the host for the connection
 * @return the file descriptor of the socket
 */
int32_t connectServer(const uint8_t* host);

/**
 * Sends the transport protocol message to the connected server, with help of the file descriptor.
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @param transportProtocol the transportProtocol message which should be send
 * @param len the length of the hole message
 * @return the status of the send
 */
int32_t sendMessage(int32_t fileDescriptor, struct TransportProtocol* transportProtocol, int32_t len);


/**
 * Reads the acknowledgment of the server that a task was created,
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @return the message which was read and send by the server. The server contains the header and single task acknowledgment.
 *         NULL if something goes wrong.
 */
struct Message* readCreateTaskServerAck(int32_t fileDescriptor);

/**
 * Uses the given file descriptor to send a create task message to the server.
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @param topic the topic of the task
 * @return the status of the send
 */
int32_t createTask(int32_t fileDescriptor, const uint8_t* topic);

int32_t pollAndLockTask(int32_t fileDescriptor, const uint8_t* topic);

struct Message* readPollAndLockServerAck(int32_t fileDescriptor);

void cleanUpMessage(struct Message* message);

#ifdef __cplusplus
}
#endif

#endif /* TNGP_H */

