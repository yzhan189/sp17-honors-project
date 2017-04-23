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
//#include <unistd.h>
int secret[8] = { 125,126,173,225,233,241,296,374 };
char* username;
char* password;
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
    char login_info[128];
    memset(login_info,0,128);
   
    char* auth=calloc(1,17);
    memset(auth,0,17);
    strcat(auth,username);
    strcat(auth," ");
    strcat(auth,password);
    write(serverSocket,auth,17);
    printf("Write auth info to server!\n");
    if(ret == -1){
      perror("connect");
      exit(1);
    }
    read(serverSocket,login_info,10);
    printf("%s\n",login_info);
    if(strncmp(login_info,"Fail!!!!",8)==0){
        exit(1);
    }
    char *msg = calloc(1, strlen(url) + 1);
    strcpy(msg, url);
    msg[strlen(url)] = '\0';

    fprintf(stderr, "BEFORE ENCRPTION:[%s]\n> strlen(url): [%lu]\n> strlen(msg): [%lu]\n",msg,strlen(url),strlen(msg));
    encrypt_(msg,strlen(msg),secret,8);
    msg[strlen(url)] = '\0';
    fprintf(stderr, "AFTER ENCRPYTION:[%s]\n> strlen(url): [%lu]\n> strlen(msg): [%lu]\n",msg,strlen(url),strlen(msg));
    // encrypt_(msg,strlen(msg),secret,8);
    // msg[strlen(url)] = '\0';
    // fprintf(stderr, "DECRPYTION:[%s]\n> strlen(url): [%lu]\n> strlen(msg): [%lu]\n",msg,strlen(url),strlen(msg));
    // if(strcmp(msg, url)) {
    //   perror("encrypt_\n");
    // };


    size_t count = strlen(msg);
    printf("SENDING [%lu] bytes: %s\n", strlen(url), msg);
    printf("===\n");
    // return 0;
    ssize_t bytewrite = 0;
    ssize_t totalwrite = 0;
    while(count > 0) {
      bytewrite = write(serverSocket , msg, strlen(msg));
      if(bytewrite == -1) {
        return -1;
      }
      count -= bytewrite;
      totalwrite += bytewrite;
    }
    fprintf(stderr, "total sent: %zu\n", totalwrite);
    shutdown(serverSocket, SHUT_WR);
    // return 0;
    // sleep(3);
    //read response
    char resp[1000];
    int len = read(serverSocket , resp, 999);
    resp[len] = '\0';
    encrypt_(resp,strlen(resp),secret,8);
    resp[strlen(resp)] = '\0';
    fprintf(stderr, "## DECRPYTION SERVER RESPONSE:[%s](lem: %lu)\n",resp,strlen(resp));
    // if(strcmp(resp, url)) {
    //   perror("decrypt\n");
    // };
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

    if (argc != 6) {
        fprintf(stderr, "Usage: %s <address> <port> <url> <username> <password>\n",
                argv[0]);
        exit(1);
    }
    username=argv[4];
    password=argv[5];

    signal(SIGINT, close_program);
    serverSocket = connect_to_server(argv[1], argv[2], argv[3]);

    return 0;
}
