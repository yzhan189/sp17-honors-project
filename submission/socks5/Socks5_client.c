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
enum {SUCCESS, AUTH_ERROR, VERSION_ERROR} status
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
int connect_to_server(const char *host, const char *port, const char *username,
                      const char *password, const char *url) {


    int s;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(struct sockaddr_in));
    saddr.sin_port = htons(1080);


    int ret = connect(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret == -1){
      perror("connect to proxy");
      exit(1);
    }


    /**
     *  GET AUTH METHOD
     */
    char buf[256];
    char *pen = buf;
    pen ++ = 5;
    pen ++ = 2;
    pen ++ = 0x00;
    pen ++ = 0x02;
    send(serverSocket, pen, buf - pen, 0);
    recv(serverSocket, buf, 2, 0);

    if(buf[1] == 0xFF) {
      perror("version not support");
      exit(-1);
    }
    else if(buf[1] == 0x02) {
      //version supported
      // send usrname
      pen = buf;
      pen ++ = 1;
      size_t username_len = strlen(username);
      pen ++ = username_len;
      strcpy(pen, username);
      pen += username_len;
      //send password
      size_t password_len = strlen(password);
      ptr ++ = password_len;

      send(serverSocket, buf, pen - buf);
      recv(serverSocket, buf, 2);

      if(buf[1] != 0) {
        perror("Authentication");
        exit(AUTH_ERROR);
      }
      // Authentication pass
      pen = buffer;
      pen++ = 5;  //socks verison
      pen++ = 1;  //sending connect cmd
      pen++ = 0;
      pen++ = 1;
      /**
       * get the address of target destination
       */
      struct addrinfo hints, *result;
      memset(&hints, 0, sizeof(struct addrinfo));

      hints.ai_family = AF_INET; /* IPv4 only */
      hints.ai_socktype = SOCK_STREAM; /* TCP */

      s = getaddrinfo(host, port, &hints, &result);
      if (s != 0) {
             fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
             exit(1);
      }

      struct addrinfo *p;
      char host[256],service[256];

      for(p = result; p != NULL; p = p->ai_next) {

        getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof(host), service, sizeof(service), NI_NUMERICHOST);
        puts(host);
      }

      freeaddrinfo(infoptr);
      memcpy(pen, )



    }

    char *msg = calloc(1, strlen(url) + 1);
    strcpy(msg, url);
    msg[strlen(url)] = '\0';

    fprintf(stderr, "BEFORE ENCRPTION:[%s]\n> strlen(url): [%lu]\n> strlen(msg): [%lu]\n",msg,strlen(url),strlen(msg));
    encrypt(msg,strlen(msg),secret,8);
    msg[strlen(url)] = '\0';
    fprintf(stderr, "AFTER ENCRPYTION:[%s]\n> strlen(url): [%lu]\n> strlen(msg): [%lu]\n",msg,strlen(url),strlen(msg));
    // encrypt(msg,strlen(msg),secret,8);
    // msg[strlen(url)] = '\0';
    // fprintf(stderr, "DECRPYTION:[%s]\n> strlen(url): [%lu]\n> strlen(msg): [%lu]\n",msg,strlen(url),strlen(msg));
    // if(strcmp(msg, url)) {
    //   perror("encrypt\n");
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
    encrypt(resp,strlen(resp),secret,8);
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

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <address> <port> <url>\n",
                argv[0]);
        exit(1);
    }


    signal(SIGINT, close_program);
    serverSocket = connect_to_server(argv[1], argv[2], argv[3]);

    return 0;
}
