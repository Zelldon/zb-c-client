/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.c
 * Author: Christopher Zell <christopher.zell@camunda.com>
 *
 * Created on August 23, 2016, 8:18 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>

#include "tngp.h"

int main(int argc, char** argv) {
  puts("Welcome to the Team8-Client\n");
  const char host[10] = "localhost\0";

  //connect with the server
  int fileDescriptor = connectServer(host);
  if (fileDescriptor < 0) {
    //failed
    perror("ERROR connection failed!");
    return (EXIT_FAILURE);
  }
  puts("Connected\nCreating task...\n");

  //create task with topic
  const char* topic = "TaskTopic";
  //  for (int i = 0; i < 3; i++) {
  int n = createTask(fileDescriptor, topic);
  if (n < 0) {
    perror("ERROR writing to socket");
    return (EXIT_FAILURE);
  }
  printf("Task with topic %s created.\n", topic);
  //  }


  //read server acknowledgment
  struct Message* serverAck = readCreateTaskServerAck(fileDescriptor);
  if (serverAck == NULL) {
    perror("ERROR reading from socket");
    return (EXIT_FAILURE);
  }
  struct SingleTaskServerAckMessage* ack = (struct SingleTaskServerAckMessage*) serverAck->body->body;
  printf("Got server acknowledgment that task with topic %s was successfully created.\nTask has id: %ld", topic, ack->taskId);
  cleanUpMessage(serverAck);

  n = pollAndLockTask(fileDescriptor, topic);
  if (n < 0) {
    perror("ERROR writing to socket");
    return (EXIT_FAILURE);
  }

  serverAck = readPollAndLockServerAck(fileDescriptor);
  if (serverAck == NULL) {
    perror("ERROR reading from socket");
    return (EXIT_FAILURE);
  }
  struct LockedTaskBatchMessage* lockedTaskBatchMessage = (struct LockedTaskBatchMessage*) serverAck->body->body;
  char b[LOCKED_TASK_BATCH_HEADER];
  memcpy(b, serverAck->body->body, LOCKED_TASK_BATCH_HEADER);
  char id = serverAck->body->body[13];
//  long* lockTime = (long*) &serverAck->body->body[2];
  printf("Polled task with id %c.", id);


  cleanUpMessage(serverAck);
  return (EXIT_SUCCESS);
}
