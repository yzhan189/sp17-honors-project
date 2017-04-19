/* *
 *  CS296 Honor Project
 */
#include "utils.h"
#include "encrypt.h"

#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
int secret[8] = { 125,126,173,225,233,241,296,374 };

static volatile int serverSocket;
void close_program(int signal);
ssize_t write_to_server(int socket, const char* url, size_t count){
  printf("SENDING: %s\n", url);
  printf("===\n");
  ssize_t bytewrite = 0;
  ssize_t totalwrite = 0;
  while(count > 0) {
    bytewrite = write(serverSocket , url, strlen(url));
    if(bytewrite == -1) {
      return -1;
    }
    count -= bytewrite;
    totalwrite += bytewrite;

  }
  return bytewrite;
}
void read_from_client(int socket);

/**
 * Shuts down connection with 'serverSocket'.
 * Called by close_program upon SIGINT.
 */
void close_server_connection() {
    // Your code here
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
}

/**
 * Sets up a connection to a chatroom server and returns
 * the file descriptor associated with the connection.
 *
 * host - Server to connect to.
 * port - Port to connect to server on.
 *
 * Returns integer of valid file descriptor, or exit(1) on failure.
 */
int connect_to_server(const char *host, const char *port, const char *url) {


    int s;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));



    hints.ai_family = AF_INET; /* IPv4 only */
    hints.ai_socktype = SOCK_STREAM; /* TCP */

    s = getaddrinfo(host, port, &hints, &result);
    if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            exit(1);
    }

    int ret = connect(serverSocket, result->ai_addr, result->ai_addrlen);
    if(ret == -1){
      perror("connect");
      exit(1);
    }
    char *origin = strdup(url);

    encrypt(origin,strlen(origin),secret,8);


    size_t count = strlen(origin);
    printf("SENDING: %s\n", origin);
    printf("===\n");
    ssize_t bytewrite = 0;
    ssize_t totalwrite = 0;
    while(count > 0) {
      bytewrite = write(serverSocket , origin, strlen(origin));
      if(bytewrite == -1) {
        return -1;
      }
      count -= bytewrite;
      totalwrite += bytewrite;
    }
    fprintf(stderr, "total sent: %zu\n", totalwrite);
    shutdown(serverSocket, SHUT_WR);

    // sleep(3);
    //read response
    char resp[1000];
    int len = read(serverSocket , resp, 999);
    resp[len] = '\0';
    /*
     *
     *  decrption goes here!
     *
     *
     */
    printf("%s\n", resp);

    return ret;
}



/*
 * Signal handler used to close this client program.
 */
void close_program(int signal) {
    if (signal == SIGINT) {
        close_server_connection();
    }
}

int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <address> <port> <url>\n",
                argv[0]);
        exit(1);
    }


    signal(SIGINT, close_program);
    serverSocket = connect_to_server(argv[1], argv[2], argv[3]);

    return 0;
}
