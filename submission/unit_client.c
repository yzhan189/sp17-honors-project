/* *
 *  CS296 Honor Project
 */
#include "utils.h"
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

static volatile int serverSocket;
// static pthread_t threads[2];

// void *write_to_server(void *arg);
// void *read_from_server(void *arg);
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
    /*QUESTION 1*/
    // 1) What is a socket?
    /*QUESTION 2*/
    // 2) What is the difference between the domains AF_INET and AF_UNIX?
    /*QUESTION 3*/
    // 3) What is the difference between SOCK_STREAM and SOCK_DGRAM?

    int s;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);


    /*QUESTION 4*/
    // 4) Why is it important to clear all the values the addrinfo struct before using it in getaddrinfo?
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));


    /*QUESTION 5*/
    // 5) What are ai_family and ai_socktype?
    hints.ai_family = AF_INET; /* IPv4 only */
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    /*QUESTION 6*/
    // 6) What does getaddrinfo do?
    s = getaddrinfo(host, port, &hints, &result);
    if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            exit(1);
    }
    /*QUESTION 7*/
    // 7) What does connect do?
    int ret = connect(serverSocket, result->ai_addr, result->ai_addrlen);
    if(ret == -1){
      perror("connect");
      exit(1);
    }
    //send info
    //char *buf = "GET / HTTP/1.0\r\n\r\n";
    size_t count = strlen(url);
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
    fprintf(stderr, "total sent: %zu\n", totalwrite);
    // sleep(3);
    //read response
    char resp[1000];
    int len = read(serverSocket , resp, 999);
    resp[len] = '\0';
    printf("%s\n", resp);

    return ret;
}



/*
 * Signal handler used to close this client program.
 */
void close_program(int signal) {
    if (signal == SIGINT) {
        // pthread_cancel(threads[0]);
        // pthread_cancel(threads[1]);
        // close_chat();
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
    // create_windows(output_filename);
    // atexit(destroy_windows);

    serverSocket = connect_to_server(argv[1], argv[2], argv[3]);

    return 0;
}
