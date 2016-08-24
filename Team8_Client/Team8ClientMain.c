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
#include <sys/socket.h>
#include<string.h>


char* createBuffer(size_t size) {
  //create buffer
  char * buffer = malloc(size);
  //init buffer
  memset(buffer, 'A', size);
  return buffer;
}

/*
 *
 */
int main(int argc, char** argv) {
  printf("Welcome to the Team8-Client\n");

  //create buffer
  char* buffer = createBuffer(512);

  //Maybe we use SOCK_SEQPACKET instead
  //SOCK_STREAM will end to TCP
  int filedescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (filedescriptor < 0) {
    return (EXIT_FAILURE);
  }

  printf("Socket opened on 50000.\n");

  struct sockaddr addr;

//  connect(filedescriptor, )





  //END
  //clean up
  free(buffer);
  return (EXIT_SUCCESS);
}

