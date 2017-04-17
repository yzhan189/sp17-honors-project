/**
 * Networking
 * CS 241 - Spring 2017
 */
#include "common.h"
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
ssize_t read_from_socket(int socket, char *buffer, size_t count) {
   // Your Code Here
   if(socket == -1 || socket == 0) {
     return 0;
   }
   char *buf = buffer;
   ssize_t total = count;
   ssize_t read = 0;
   while(read < total) {
       ssize_t byteread = recv(socket, buf, count, 0);
       if(byteread == 0) {
         break;
       }
       else if(byteread == -1) {
         if(errno == EINTR) {
           continue;
         }
         return -1;
       }
       read += byteread;
       buf += (size_t)byteread;
       count -= (size_t)byteread;
   }
   return read;

}

ssize_t write_to_socket(int socket, const char *buffer, size_t count) {
   // Your Code Here
   if(socket == -1 || socket == 0) {
     return 0;
   }
   ssize_t written = 0;
   ssize_t total = count;
   char *buf = (char *)buffer;
   while(written < total) {
     ssize_t  bytewrite = send(socket, buf, count, 0);
     if(bytewrite == 0) {
       break;
     }
     else if(bytewrite == -1) {
         if(errno == EINTR) {
             continue;
         }
         return -1;
     }

     count -= (size_t)bytewrite;
     written += bytewrite;
     buf += bytewrite;

   }
   return written;
}
