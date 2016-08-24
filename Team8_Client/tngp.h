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

#define PORT 51015


/**
 * Connects to the server with the given parameter.
 * Uses the PORT makro for the connection port.
 *
 * @param host the host for the connection
 * @return the file descriptor of the socket
 */
int connectServer(const char* host);

struct Message {
  int len;
  char version;
  char flags;
  short type;
  int streamId;
  char* msg;
};

/**
 * Sends the message to the connected server, with help of the file descriptor.
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @param msg the message which should be send
 * @return the status of the send
 */
int sendMessage(int fileDescriptor, char* msg);


#define header_len 12


#ifdef __cplusplus
}
#endif

#endif /* TNGP_H */

