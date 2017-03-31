#include<stdio.h>
#include <string.h>
/**
  *@para encrypted
  *  the size of encrypted should be at least len, otherwise it couldn't hold enough data
  *  and undefined behavior will happen
  *@para origin
  *   the data expected to be encrypted
  *@para len
  *   the size of origin
**/
void encrypt(char* origin,size_t len,int * key,int key_len){
    int i;
    for(i = 0; i <len; i++){
        origin[i] = origin[i]^key[i%key_len];
    }
}
int main(){
    int secret[8] = { 125,126,173,225,233,241,296,374 };
  char* origin=strdup("This is the original string");
  printf("origin:%s\n",origin );
  encrypt(origin,strlen(origin),secret,8);
  printf("encrypted:%s\n",origin);
  encrypt(origin,strlen(origin),secret,8);
  printf("decrypted:%s\n",origin);
}
