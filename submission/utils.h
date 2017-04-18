/* *
 *  CS296 Honor Project
 */
// #include "common.h"
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
ssize_t read_from_socket(int socket, char *buffer, size_t count);
ssize_t write_to_socket(int socket, const char *buffer, size_t count);
