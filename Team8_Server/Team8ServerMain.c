/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 51015

void dostuff(int); /* function prototype */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno, clilen, pid;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");
  memset((char*) &serv_addr, 0, sizeof (serv_addr));
  portno = PORT;


  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof (serv_addr)) < 0)
    error("ERROR on binding");
  listen(sockfd, 5);
  clilen = sizeof (cli_addr);
  while (1) {
    newsockfd = accept(sockfd,
            (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    pid = fork();
    if (pid < 0)
      error("ERROR on fork");
    if (pid == 0) {
      close(sockfd);
      dostuff(newsockfd);
      exit(0);
    } else close(newsockfd);
  } /* end of while */
  return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff(int sock) {
  int n;
  char buffer[256];

  memset(buffer, 0, 256);
  n = read(sock, buffer, 255);
  if (n < 0) error("ERROR reading from socket");
  printf("Here is the message: %s\n", buffer);
  n = write(sock, "I got your message", 18);
  if (n < 0) error("ERROR writing to socket");
}