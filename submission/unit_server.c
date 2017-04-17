/**
 * Chatroom Lab
 * CS 241 - Spring 2017
 */
#include <arpa/inet.h>
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
#include <sys/types.h>
#include <ifaddrs.h>

#define MAX_CLIENTS 8

// void *process_client(void *p);

static volatile int endSession;

static volatile int clientsCount;
static volatile int clients[MAX_CLIENTS];

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void printmyip(int required_family) {
    // required_family = AF_INET6 or AF_INET; // Change to AF_INET6 for IPv6
    struct ifaddrs *myaddrs, *ifa;
    getifaddrs(&myaddrs);
    char host[256], port[256];
    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {
        int family = ifa->ifa_addr->sa_family;
        if (family == required_family && ifa->ifa_addr) {
            if (0 == getnameinfo(ifa->ifa_addr,
                                (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                sizeof(struct sockaddr_in6),
                                host, sizeof(host), port, sizeof(port)
                                 , NI_NUMERICHOST | NI_NUMERICSERV  )) {
                puts(host);
            }
        }
    }
}
/**
 * Signal handler for SIGINT.
 * Used to set flag to end server.
 */
void close_server() {
  fprintf(stderr, "close!\n");
    endSession = 1;
    // add any additional flags here you want.
}

/**
 * Cleanup function called in main after `run_server` exits.
 * Server ending clean up (such as shutting down clients) should be handled
 * here.
 */
void cleanup() {
    // Your code here.
    int i;
    for(i = 0; i< MAX_CLIENTS; i ++) {
      shutdown(clients[i] , SHUT_RDWR);
      close(clients[i]);
    }
}

/**
 * Sets up a server connection.
 * Does not accept more than MAX_CLIENTS connections.  If more than MAX_CLIENTS
 * clients attempts to connects, simply shuts down
 * the new client and continues accepting.
 * Per client, a thread should be created and 'process_client' should handle
 * that client.
 * Makes use of 'endSession', 'clientsCount', 'client', and 'mutex'.
 *
 * port - port server will run on.
 *
 */
void run_server(char *port) {
    /*QUESTION 1*/
    // 1) What is a socket?
    /*QUESTION 2*/
    // 2) What is the difference between the domains AF_INET and AF_UNIX?
    /*QUESTION 3*/
    // 3) What is the difference between SOCK_STREAM and SOCK_DGRAM?
    // signal(SIGINT, close_server);
    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    /*QUESTION 8*/
    // 8) What is setsockopt?
    int optval = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    /*QUESTION 4*/
    // 4) Why is it important to clear all the values the addrinfo struct before using it in getaddrinfo?
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    /*QUESTION 5*/
    // 5) What are ai_family and ai_socktype?
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /*QUESTION 6*/
    // 6) What does getaddrinfo do?
    s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    /*QUESTION 9*/
    // 9) What does bind do?
    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind()");
        exit(1);
    }

    /*QUESTION 10*/
    // 10) What does listen do?
    if (listen(sock_fd, 10) != 0) {
        perror("listen()");
        exit(1);
    }
    struct sockaddr_in *result_addr = (struct sockaddr_in *) result->ai_addr;
    printf("Listening on file descriptor %d, port %d\n", sock_fd, ntohs(result_addr->sin_port));

    while(1)
    {
      printf("Waiting for connection...\n");
      pthread_mutex_lock(&mutex);
      clients[clientsCount] = accept(sock_fd, NULL, NULL);
      pthread_mutex_unlock(&mutex);



      printf("Connection made: client_fd=%d\n", clients[clientsCount]);

      char buffer[1000];
      int len = read(clients[clientsCount], buffer, sizeof(buffer) - 1);
      buffer[len] = '\0';

      printf("Read %d chars\n", len);
      printf("===\n");
      printf("%s\n", buffer);

      // sleep(3);
      char *resp = "Got yo!";
      write(clients[clientsCount] , resp, strlen(resp));

      pthread_mutex_lock(&mutex);
      clientsCount ++;
      pthread_mutex_unlock(&mutex);


    }
}



int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "./server <port>\n");
        return -1;
    }
    printmyip(AF_INET);
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = close_server;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }
    signal(SIGINT, close_server);
    run_server(argv[1]);
    cleanup();
    pthread_exit(NULL);
}
