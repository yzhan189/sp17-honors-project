/* *
 *  CS296 Honor Project
 */
#include "utils.h"
#include "encrypt.h"

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
int secret2[8] = { 125,126,173,225,233,241,296,374 };
//
// void encrypt_(char* origin,size_t len,int * key,int key_len){
//     int i;
//     for(i = 0; i <len; i++){
//         origin[i] = origin[i]^key[i%key_len];
//     }
// }
char *url = "www.google.com";
char *username;
char *password;
#define MAX_CLIENTS 8
void* connect_to_remote(void *p) ;

static volatile int endSession;
static volatile int serverSocket;
static volatile int clientsCount;
static volatile int clients[MAX_CLIENTS];
pthread_t tid[MAX_CLIENTS];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct addrinfo *result;

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
 * Cleanup function called in main after `run_server` exits.
 * Server ending clean up (such as shutting down clients) should be handled
 * here.
 */
void cleanup() {
    // Your code here.
    int i;
    for(i = 0; i< MAX_CLIENTS; i ++) {
      pthread_cancel(tid[i]);
      if(clients[i] != -1) {
        shutdown(clients[i] , SHUT_RDWR);
        close(clients[i]);
      }
    }
    free(username);
    free(password);
    shutdown(serverSocket , SHUT_RDWR);
    close(serverSocket);
}
/**
 * Signal handler for SIGINT.
 * Used to set flag to end server.
 */
void close_server() {
    fprintf(stderr, "close!\n");
    endSession = 1;
    cleanup();
    freeaddrinfo(result);
    pthread_exit(NULL);
}
/**
  *the username and the password must be both 8 bytes and seperated by a space
  *since the server will only read 17 bytes for authentication
  */

void read_user_password(char* filename){
	FILE* f=fopen(filename,"r");
	char* line;
	size_t len;
	getline(&line, &len,f);
    char* origin=line;
	char* user=strsep(&line," ");
	printf("user:%s password:%s",user,line);
    username=strndup(user,8);
    password=strndup(line,8);
    free(origin);
}

int authentication(char* buffer){
    char* user=strsep(&buffer," ");
    printf("the username sent from clinet is %s and the password is %s\n",user,buffer);
    if(strncmp(username,user,8)==0&&strncmp(password,buffer,8)==0){
        return 1;
    }
    else{
        return 0;
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

    int s;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);


    int optval = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    if (bind(serverSocket, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind()");
        exit(1);
    }

    if (listen(serverSocket, 10) != 0) {
        perror("listen()");
        exit(1);
    }
    struct sockaddr_in *result_addr = (struct sockaddr_in *) result->ai_addr;
    printf("Listening on file descriptor %d, port %d\n", serverSocket, ntohs(result_addr->sin_port));
    while(!endSession) {

      pthread_mutex_lock(&mutex);
      if(clientsCount >= MAX_CLIENTS) {
        pthread_mutex_unlock(&mutex);

        continue;
      }
      int curr_client = clientsCount;
      pthread_mutex_unlock(&mutex);

      printf("Waiting for connection...\n");
      struct sockaddr client_addr;
      bzero(&client_addr, sizeof(struct sockaddr));
      socklen_t socklen = (socklen_t)sizeof(struct sockaddr);
      int client_fd = accept(serverSocket, &client_addr, &socklen);
      printf("Connection made: client_fd=%d\n", client_fd);
      printf("Now start to authenticate!\n");
      char buffer[1024];
      read(client_fd,buffer,17);
      int auth=authentication(buffer);
      if(auth==1){
          printf("Success!\n" );
          write(client_fd,"Success!",8);
      }
      else{
          write(client_fd,"Fail!!!!",8);
          printf("Fail! closing this connection\n" );
          close(client_fd);
      }
      pthread_mutex_lock(&mutex);
      clients[curr_client] = client_fd;
      // hand off to function
      clientsCount ++;
      pthread_mutex_unlock(&mutex);
      if(!pthread_create(&tid[curr_client], NULL, connect_to_remote, (void *)(intptr_t)curr_client)) {
        perror("thread");
        continue;
      }

    }
    freeaddrinfo(result);
}

void* connect_to_remote(void *p) {
    pthread_detach(pthread_self());
    intptr_t clientId = (intptr_t)p;
    ssize_t retval = 1;
    char *buffer = NULL;
    /*
     *  read request from client
     */
    char resp[1000];
    int len1 = read(clients[clientId], resp, 999);
    resp[len1] = '\0';
    fprintf(stderr, "## RAW MESSAGE RECEIVED: %s\n", resp);
    /*
     * decrption
     */
    // char *msg = calloc(1, len1);
    encrypt_(resp,strlen(resp),secret2,8);
    // [strlen(url)] = '\0';
    fprintf(stderr, "DECRPYTION:[%s](len: %lu)\n",resp,strlen(resp));
    if(strcmp(resp, url)) {
      perror("encrypt_\n");
    };
    // return NULL;
    int s;
    int out_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /* IPv4 only */
    hints.ai_socktype = SOCK_STREAM; /* TCP */

    s = getaddrinfo(resp, "80", &hints, &result);
    if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            exit(1);
    }

    if(connect(out_serverSocket, result->ai_addr, result->ai_addrlen) == -1){
                perror("connect");
                exit(2);
    }


    char *request = "GET / HTTP/1.0\r\n\r\n";
    printf("SENDING: %s", request);
    printf("===\n");
    write(out_serverSocket, request, strlen(request));

    int len = 0;
    int success = 0;
    do {
        char resp[1000] = {0};
        len = read(out_serverSocket, resp, 999);
        resp[len] = '\0';
        /*
         *
         *  encrption goes here!
         *
         *
         */
         encrypt_(resp,strlen(resp),secret2,8);
         resp[len] = '\0';
         fprintf(stderr, "## RAW SERVER RESPONSE: [%s](len: %lu)\n", resp, strlen(resp));
         write(clients[clientId], resp, strlen(resp));
         if(len) {
            if(success == 0) {
                success = 1;
            }
          }
        // printf("%s\n", resp);
    }while(len);

    // if(success) {
    //     char *buffer = "Connection made successfully!\r\n\r\n";
    //     // printf("SENDING BACK TO CLIENT: %s", buffer);
    //     printf("===\n");
    //     write(clients[clientId], buffer, strlen(buffer));
    // }

    // return 0;





    printf("User %d left\n", (int)clientId);
    close(clients[clientId]);

    pthread_mutex_lock(&mutex);
    clients[clientId] = -1;
    clientsCount--;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "./server <port> <file>\n");
        return -1;
    }
	read_user_password(argv[2]);
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
