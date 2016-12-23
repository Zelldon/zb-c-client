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
 * File:   main.c
 * Author: Christopher Zell <christopher.zell@camunda.com>
 *
 * Created on August 23, 2016, 8:18 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tngp.h"

/**
 * Connect with the broker.
 *
 * @return returns the file descriptor
 */
int connectToBroker(const char* host) {
  int fileDescriptor = connectServer(host);
  if (fileDescriptor < 0) {
    //failed
    perror("ERROR connection failed!");
    exit(EXIT_FAILURE);
  }
  puts("Connected\nCreating task...\n");
  return fileDescriptor;
}

/**
 * Creates a task on the broker.
 *
 * @param fileDescriptor the file descriptor to connect with the broker
 * @param topic the topic of the task
 */
void createTaskOnBroker(int fileDescriptor, const char* topic) {

  struct SingleTaskServerAckMessage* ack = createTask(fileDescriptor, topic);
  if (ack == NULL) {
    freeSingleTaskServerAckMessage(ack);
    perror("ERROR writing to socket");
    exit(EXIT_FAILURE);
  }

  printf("Got server acknowledgment that task with topic %s was successfully created.\nTask has id: %ld", topic, ack->taskId);
  freeSingleTaskServerAckMessage(ack);
}

/**
 * Poll and lock a task on the broker.
 *
 * @param fileDescriptor the file descriptor to connect with the broker
 * @param topic the topic of the task which should be polled and locked
 */
void pollAndLockOnBroker(int fileDescriptor, const char* topic) {
  struct LockedTaskBatchMessage* ack = pollAndLockTask(fileDescriptor, topic);
  if (ack == NULL) {
    perror("ERROR writing to socket");
    exit(EXIT_FAILURE);
  }

  printf("Got server acknowledgment that task with topic %s was successfully locked.\n", topic);
  printf("Task has id: %ld locked for %ld", ack->taskId, ack->lockTime);
  freeLockedTaskBatchMessage(ack);
//  int n = pollAndLockTask(fileDescriptor, topic);
//  if (n < 0) {
//    perror("ERROR writing to socket");
//    exit(EXIT_FAILURE);
//  }
//
//  struct Message* serverAck = readPollAndLockServerAck(fileDescriptor);
//  if (serverAck == NULL) {
//    perror("ERROR reading from socket");
//    exit(EXIT_FAILURE);
//  }
//  struct LockedTaskBatchMessage* lockedTaskBatchMessage = (struct LockedTaskBatchMessage*) serverAck->body->body;
//  char b[LOCKED_TASK_BATCH_HEADER];
//  memcpy(b, serverAck->body->body, LOCKED_TASK_BATCH_HEADER);
//  char id = serverAck->body->body[13];
////  long* lockTime = (long*) &serverAck->body->body[2];
//  printf("Polled task with id %c.", id);
//
//  cleanUpMessage(serverAck);
}

int main(int argc, char** argv) {
  puts("Welcome to the Team8-Client\n");
  const char host[10] = "localhost\0";

  //connect with the server
  int fileDescriptor = connectToBroker(host);

  //create task with topic
  const char* topic = "TaskTopic";
  createTaskOnBroker(fileDescriptor, topic);

  //poll and lock
  pollAndLockOnBroker(fileDescriptor, topic);

  return (EXIT_SUCCESS);
}
