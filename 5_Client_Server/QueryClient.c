//
// Created by Khai Ly on 4/17/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "includes/QueryProtocol.h"

char *port_string = "1500";
char *ip = "127.0.0.1";

#define BUFFER_SIZE 1000
#define QUERY_LIMIT 100

void RunQuery(char *query) {
  // Find the address
  int sockfd, numBytes, err;
  char data[BUFFER_SIZE];
  struct addrinfo local, *result;

  memset(&local, 0, sizeof local);
  local.ai_family = AF_UNSPEC;
  local.ai_socktype = SOCK_STREAM;
  // establishing connection
  // if fail to connect with server, intentially terminate.
  if ((err = getaddrinfo(ip, port_string, &local, &result)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
    printf("Please make sure server is up and you"
            " have inputed correct IP address and "
            "port number in commandline.\n");
    printf("Enter 'q' at next search to exit and start new connection.\n");
    return;
  }
  // Get socket
  if ((sockfd = socket(result -> ai_family,
      result -> ai_socktype,
      result -> ai_protocol)) == -1) {
    perror("client: socket error");
    exit(1);
  }
  // if fail to connect with server, intentionally terminate.
  // valid command line input here as well.
  if (connect(sockfd, result -> ai_addr, result -> ai_addrlen) == -1) {
    close(sockfd);
    freeaddrinfo(result);
    perror("Message from server");
    printf("Please make sure server is up and you"
            " have inputed correct IP address and "
            "port number in commandline.\n");
    printf("Enter 'q' at next search to exit and start new connection.\n");
    return;
  }
  freeaddrinfo(result);
  // receiv ack from server
  if ((numBytes = recv(sockfd, data, BUFFER_SIZE - 1, 0)) == -1) {
    perror("client: recv");
    exit(1);
  }
  data[numBytes] = '\0';
  if (CheckAck(data) != 0) {
    perror("client: no acknowledge from server");
    exit(1);
  }

  // client sends query to server.
  if (send(sockfd, query, strlen(query), 0) == -1) {
    perror("server: send error");
    exit(1);
  }
  // client receives number of data from server.
  if ((numBytes = recv(sockfd, data, BUFFER_SIZE - 1, 0)) == -1) {
    perror("client: recv");
    exit(1);
  }
  data[numBytes] = '\0';
  printf("Number of movies are: %s \n", data);
  int num = atoi(data);

  // client sends ack to server
  if (SendAck(sockfd) != 0) {
    perror("client: could not send ACK to server.");
    exit(1);
  }
  // client prints out result
  for (int i = 0; i < num; i++) {
    if ((numBytes = recv(sockfd, data, BUFFER_SIZE - 1, 0)) == -1) {
      perror("client: recv");
      exit(1);
    }
    data[numBytes] = '\0';
    printf("%s\n", data);
    if (SendAck(sockfd) != 0) {
      perror("client: could not send ACK to server.");
      exit(1);
    }
  }
  if ((numBytes = recv(sockfd, data, BUFFER_SIZE - 1, 0)) == -1) {
    perror("client: recv");
    exit(1);
  }
  data[numBytes] = '\0';
  if (CheckGoodbye(data) != 0) {
    printf("Server sent request to bye.\n");
    exit(1);
  }
  close(sockfd);
  return;
}

void RunPrompt() {
  char input[BUFFER_SIZE];

  while (1) {
    printf("\n================================\n");
    printf("Welcome to our database! Search a term"
          " to see all media with that term in our"
          " databse.\n");
    printf("Enter a term to search for, or q to quit: ");
    scanf("%s", input);

    printf("input was: %s\n", input);

    if (strlen(input) == 1) {
      if (input[0] == 'q') {
        printf("Thanks for playing!\n");
        printf("==================================\n");
        return;
      }
    }
    // limit the length of query to 100 characters
    if (strlen(input) >= QUERY_LIMIT) {
      printf("Search term can only have maximum 100 characters.\n");
      continue;
    }
    printf("\n\n");
    RunQuery(input);
  }
}

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Number of arguments must be 3.\n");
    printf("For example: \"./queryclient localhost 1500\"\n");
    exit(1);
  }
  // Checking for valid IP address are done in RunPrompt
  // Checking for valid port here
  int port = atoi(argv[2]);
  if (port > 65536 || port < 0) {
    printf("Please make sure your port is correct."
           " Port can only be from 0 to 65546.\n");
    return -1;
  }
  if (strcmp(argv[1], "127.0.0.1") != 0) {
    ip = argv[1];
  }
  if (strcmp(argv[2], port_string) != 0) {
    port_string = argv[2];
  }

  RunPrompt();

  return 0;
}
