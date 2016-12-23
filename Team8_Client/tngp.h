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
#include "protocol.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

//util
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
int32_t connectServer(const uint8_t* host);

/**
 * Uses the given file descriptor to send a create task message to the server.
 *
 * @param fileDescriptor the file descriptor of the connected socket
 * @param topic the topic of the task
 * @return the status of the send
 */
struct SingleTaskServerAckMessage* createTask(int32_t fileDescriptor, const uint8_t* topic);

struct LockedTaskBatchMessage* pollAndLockTask(int32_t fileDescriptor, const uint8_t* taskTopic);

#ifdef __cplusplus
}
#endif

#endif /* TNGP_H */

