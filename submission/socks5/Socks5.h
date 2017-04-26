#ifndef SOCKS5_H
#define SOCKS5_H

#define VERSION 0x05
#define CONNECT 0x01
#define IPV4 0x01
#define DOMAIN 0x03
#define IPV6 0x04

/*
 * server response
 */
typedef struct _method_select_response
{
  char version; // Socks version:0x04 or 0x05
  char select_method;// 0x00: anonamys，0x02: password
} METHOD_SELECT_RESPONSE;
/*
 * server request
 */
typedef struct _method_select_request
{
  char version; // Socks version:0x04 or 0x05
  char number_methods; //support method counts
  char methods[255]; // method type: 0x00: anonamys，0x02: password
} METHOD_SELECT_REQUEST;
/*
 *  authentication response
 */
 typedef struct _AUTH_RESPONSE
 {
   char version;// 0x01
   char result;// 0x00: success, o.w, fail
 } AUTH_RESPONSE;
 /*
  *  authentication request
  */
 typedef struct _AUTH_REQUEST
 {
   char version; // 0x01
   char name[255]; // username
   char name_len;
   char pwd[255]; // password
   char pwd_len;
 } AUTH_REQUEST;
 /*
  *  sock5 response
  */
 typedef struct _SOCKS5_RESPONSE // connect to remote
 {
   char version; // Socks version:0x04 or 0x05
   char reply; // 0x00: success, o.w, fail
   char reserved; // 0x00
   char address_type; // address type: IP V4:0x01,IP V6:0x04，addr: 0x03
   char address_port[1];
 }SOCKS5_RESPONSE;
 /*
  *  sock5 request
  */
 typedef struct _SOCKS5_REQUEST // connect to remote
 {
   char version;  // Socks version:0x04 or 0x05
   char cmd; // 0x00: success, o.w, fail
   char reserved; // 0x00
   char address_type; // address type: IP V4:0x01,IP V6:0x04，addr: 0x03
 }SOCKS5_REQUEST;
 #endif
