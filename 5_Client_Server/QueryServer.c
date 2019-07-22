//
// Created by Khai Ly on 4/17/2019.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>


#include "QueryProtocol.h"
#include "MovieSet.h"
#include "MovieIndex.h"
#include "DocIdMap.h"
#include "htll/Hashtable.h"
#include "QueryProcessor.h"
#include "FileParser.h"
#include "FileCrawler.h"

DocIdMap docs;
Index docIndex;

char *port_string = "1500";
char *ip = "127.0.0.1";
#define BACKLOG 10
#define BUFFER_SIZE 1000
#define SEARCH_RESULT_LENGTH 1500
char movieSearchResult[SEARCH_RESULT_LENGTH];
void RecvCustom(int sockfd);
char buf[BUFFER_SIZE];
void runQuery(char* term);

void SendGoodbyeCustom(int new_fd);
void CheckAckCustom();
void SendCustom(int new_fd, char* msg);
int ServerConnect();
int Cleanup();
int HandleConnections(int sockfd);

void sigint_handler(int sig) {
  write(0, "Exit signal sent. Cleaning up...\n", 34);
  Cleanup();
  exit(0);
}


void Setup(char *dir) {
  printf("Crawling directory tree starting at: %s\n", dir);
  // Create a DocIdMap
  docs = CreateDocIdMap();
  CrawlFilesToMap(dir, docs);
  printf("Crawled %d files.\n", NumElemsInHashtable(docs));

  // Create the index
  docIndex = CreateIndex();

  // Index the files
  printf("Parsing and indexing files...\n");
  ParseTheFiles(docs, docIndex);
  printf("%d entries in the index.\n", NumElemsInHashtable(docIndex->ht));
}

int Cleanup() {
  DestroyOffsetIndex(docIndex);
  DestroyDocIdMap(docs);

  return 0;
}


void RecvCustom(int new_fd) {
  int numBytes;
  if ((numBytes = recv(new_fd, buf, BUFFER_SIZE - 1, 0)) == -1) {
    perror("Server: recv error");
    exit(1);
  }
  buf[numBytes] = '\0';
}

void SendCustom(int new_fd, char* msg) {
  if (send(new_fd, msg, strlen(msg), 0) == -1) {
    perror("client: send result error");
    exit(1);
  }
}

void CheckAckCustom() {
  if (CheckAck(buf) != 0) {
    perror("Server: no ack from client");
    exit(1);
  }
}

void SendGoodbyeCustom(int new_fd) {
  if (SendGoodbye(new_fd) != 0) {
    perror("Failed to send good bye");
    exit(1);
  }
}

void SendAckCustom(int new_fd) {
  if (SendAck(new_fd) == -1) {  // Send ACK to client.
    perror("Ack error");
    exit(1);
  }
}

int HandleConnections(int sockfd) {
  int new_fd;
  socklen_t sin_size;
  struct sockaddr_storage other;
  while (1) {
    sin_size = sizeof other;
    new_fd = accept(sockfd, (struct sockaddr*) &other, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    printf("===========================================\n"
          "Connection made: client_fd=%d\n", new_fd);
    // Send ACK to client.
    SendAck(new_fd);

    // Server receive first query term
    RecvCustom(new_fd);

    printf("===========================================\n"
          "Server: receive query '%s' from client\n", buf);
    printf("===========================================\n");
    SearchResultIter results = FindMovies(docIndex, buf);

    // If no results
    if (results == NULL) {
      printf("No results for this term. Please try another.\n");
      char numChar[2] = {'0', '\0'};
      SendCustom(new_fd, numChar);
      RecvCustom(new_fd);
      SendGoodbyeCustom(new_fd);
    } else {
      // If there are results
      int num = NumResultsInIter(results);
      char numChar[BUFFER_SIZE];
      snprintf(numChar, BUFFER_SIZE, "%d", num);

      // server sends number of results to client.
      SendCustom(new_fd, numChar);

      // server receives first ACK from client.
      RecvCustom(new_fd);
      CheckAckCustom();
      SearchResult sr = (SearchResult) malloc(sizeof(*sr));
      if (sr == NULL) {
        printf("Couldn't malloc SearchResult in main.c\n");
        exit(1);
      }
      SearchResultGet(results, sr);
      if (CopyRowFromFile(sr, docs, movieSearchResult) != 0) {
        printf("Couldn't get movie rows from file");
        exit(1);
      }

      // server sends first result
      SendCustom(new_fd, movieSearchResult);

      // check ACK from client after sending result;
      RecvCustom(new_fd);
      CheckAckCustom();

      // server sends more result
      while (SearchResultIterHasMore(results) != 0) {
        int x = SearchResultNext(results);
        if (x < 0) {
          printf("Error retrieving result\n");
          break;
        }
        SearchResultGet(results, sr);
        if (CopyRowFromFile(sr, docs, movieSearchResult) != 0) {
          printf("Couldn't get movie rows from file");
          exit(1);
        }
        SendCustom(new_fd, movieSearchResult);
        RecvCustom(new_fd);
        CheckAckCustom();
      }
      // send good bye to client;
      SendGoodbyeCustom(new_fd);
      free(sr);
      DestroySearchResultIter(results);
    }
    close(new_fd);
  }
}

int ServerConnect() {
  int sockfd, rt, yes = 1;
  struct addrinfo local, *result, *loop;

  memset(&local, 0, sizeof local);
  local.ai_family = AF_UNSPEC;
  local.ai_socktype = SOCK_STREAM;
  if ((rt = getaddrinfo(ip, port_string, &local, &result)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rt));
    return 1;
  }
  // check for null address;
  for (loop = result; loop != NULL; loop = loop -> ai_next) {
    if ((sockfd = socket(loop -> ai_family, loop -> ai_socktype,
            loop -> ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
          sizeof(yes)) == -1) {
      perror("Server: Error address already in used");
      exit(1);
    }
    if (bind(sockfd, loop -> ai_addr, loop -> ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    break;
  }
  freeaddrinfo(result);

  if (loop == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }
  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
  printf("Server: waiting for connections...\n");
  return sockfd;
}
int main(int argc, char **argv) {
  // Get args
  // Setup graceful exit
  struct sigaction kill;
  kill.sa_handler = sigint_handler;
  kill.sa_flags = 0;  // or SA_RESTART
  sigemptyset(&kill.sa_mask);
  if (sigaction(SIGINT, &kill, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
  // Check if commandline inputs are correct.
  if (argc != 3) {
    fprintf(stderr, "Number of arguments must be 3.\n");
    printf("For example: \"./queryserver data_small/ 1500\"\n");
    exit(1);
  }

  // check if port is valid.
  int port = atoi(argv[2]);
  if (port > 65536 || port < 0) {
    printf("======================================\n"
        "Please make sure your port is correct."
        "\nPort can only be from 0 to 65546.\n"
        "=====================================\n");
    return -1;
  }
  // check if arg[1] is a valid folder.
  if (access(argv[1], F_OK) != -1) {
    Setup(argv[1]);
  } else {
    printf("============================\n");
    printf("Directory does not exist.\n");
    printf("Please make sure you input correct folder"
          " following './queryserver'\n");
    printf("Example: './queryserver data_small/ 1500'\n");
    printf("============================\n");
    return -1;
  }

  int sockfd;
  sockfd = ServerConnect();
  HandleConnections(sockfd);
  close(sockfd);
  Cleanup();

  return 0;
}
