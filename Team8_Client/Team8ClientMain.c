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

  //init
  const char host[10] = "localhost\0";
  //connect
  int filedescriptor = connectServer(host);
  if (filedescriptor < 0) {
    //failed
    perror("ERROR connection failed!");
    return (EXIT_FAILURE);
  }
  puts("Connected\nSending message...");

//  for (int i = 0; i < 3; i++) {
    int n = createTask(filedescriptor);// sendMessage(filedescriptor, message);
    if (n < 0) {
      perror("ERROR writing to socket");
      return (EXIT_FAILURE);
    }
//  }

  return (EXIT_SUCCESS);
}
