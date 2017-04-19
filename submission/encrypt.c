/* *
 *  CS296 Honor Project
 */


#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void encrypt(char* origin,size_t len,int * key,int key_len);
