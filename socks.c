#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#include <fcntl.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socks.h"

static socks5_cfg_t g_cfg={0};
static int32_t socks5_srv_init(uint16_t port, int32_t backlog);
static int32_t socks5_srv_exit();

static int32_t socks5_sockset(int sockfd);
int main(int argc,char ** argv){
  //first argument is the port number
  g_cfg.port=atoi(argv[1]);
  g_cfg.fd=socks5_srv_init(g_cfg.port,10);
  if(g_cfg.fd==-1){
    fprintf(stderr,"failed to initialize socks server!");
    return -1;
  }
  g_cfg.state=SOCKS5_STATE_RUNNING;
  while(1){
    //never exit

  }
  fprintf(stderr, "exit server!\n" );
  return 0;
}
static int32_t socks5_srv_init(uint16_t port, int32_t backlog) {
    struct sockaddr_in serv;
    int sockfd;
    int opt;
    int flags;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error!\n");
        return -1;
    }
    bzero((char *)&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(port);
    if (-1 == (flags = fcntl(sockfd, F_GETFL, 0))) {
        flags = 0;
    }
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    opt = 1;
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (uint *)&opt, sizeof(opt))) {
        fprintf(stderr, "setsockopt SO_REUSEADDR fail.\n" );
        return -1;
    }
    if (bind(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        fprintf(stderr, "bind error [%d]\n", errno);
        return -1;
    }

    if (listen(sockfd, backlog) < 0) {
        fprintf(stderr, "Listen error!\n" );
        return -1;
    }

    return sockfd;
}
