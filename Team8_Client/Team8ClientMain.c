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


char* createBuffer(size_t size) {
  //create buffer
  char * buffer = malloc(size);
  //init buffer
  memset(buffer, 0, size);
  return buffer;
}


int main(int argc, char** argv) {
  puts("Welcome to the Team8-Client\n");

  //init
  //create buffer
  char* buffer = createBuffer(512);
  const char host[10] = "localhost\0";
  //connect
  int filedescriptor = connectServer(host);
  if (filedescriptor < 0) {
    //failed
    perror("ERROR connection failed!");
    return (EXIT_FAILURE);
  }
  puts("Connected\nSending message...");

  //write message
  //  memset(buffer, 0, 256);

  for (int i = 0; i < 3; i++) {
    char message[16] = "Test 1234567890\0";
    int n = sendMessage(filedescriptor, message);
    if (n < 0) {
      perror("ERROR writing to socket");
      return (EXIT_FAILURE);
    }
  }

  //read response
//  memset(buffer, 0, 256);
//  n = read(filedescriptor, buffer, 255);
//  if (n < 0) {
//    perror("ERROR reading from socket");
//    return (EXIT_FAILURE);
//  }
//  printf("%s\n", buffer);

  //END
  //clean up
  free(buffer);
  return (EXIT_SUCCESS);
}
