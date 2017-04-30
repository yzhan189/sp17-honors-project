#ifndef SOCKS5_H
#define SOCKS5_H

#define SOCKS5_VERSION 0x05
#define SOCKS5_CONNECT 0x01
#define SOCKS5_IPV4 0x01
#define SOCKS5_DOMAIN 0x03
#define SOCKS5_IPV6 0x04

/**
* server macro
*/
#define MAX_USER 10
#define BUFF_SIZE 1024

#define AUTH_CODE 0x02

#define TIME_OUT 6000000

#define MIN -13



typedef enum _socks5_status {
	SOCKS5_ERROR_SOCKET = MIN,
	SOCKS5_ERROR_BIND,
	SOCKS5_ERROR_LISTEN,
	SOCKS5_ERROR_CONNECT,
	SOCKS5_ERROR_GETADDRINFO,
	SOCKS5_ERROR_THREAD,
	SOCKS5_ERROR_SEND,
	SOCKS5_ERROR_RECV,
	SOCKS5_ERROR_METHOD,
	SOCKS5_ERROR_AUTH,
	SOCKS5_ERROR_ACKCMD,
	SOCKS5_ERROR_USAGE,
	SOCKS5_ERROR_TARGET,
	SOCKS5_SUCCESS
} SOCKS5_STATUS;


/*
* server response
*/
typedef struct _METHOD_SELECT_RESPONSE {
	char version; // field 1: SOCKS version, 1 byte (0x05 for this version)
	char select_method;//field 2: chosen authentication method, 1 byte, or 0xFF if no acceptable methods were offered
} METHOD_SELECT_RESPONSE;
/*
* server request
*/
typedef struct _METHOD_SELECT_REQUEST {
	char version; // field 1: SOCKS version number (must be 0x05 for this version)
	char number_methods; //field 2: number of authentication methods supported, 1 byte
	char methods[255]; // field 3: authentication methods, variable length, 1 byte per method supported
} METHOD_SELECT_REQUEST;
/*
*  authentication response
*/
typedef struct _AUTH_RESPONSE  {
	char version;// field 1: version, 1 byte
	char result;//field 2: status code, 1 byte
		// 0x00 = success
		// any other value = failure, connection must be closed
} AUTH_RESPONSE;
/*
*  authentication request
*/
typedef struct _AUTH_REQUEST  {
	char version; //field 1: version number, 1 byte (must be 0x01)
	char username_len;//field 2: username length, 1 byte
	char username[255]; //field 3: username
	char password_len; //field 4: password length, 1 byte
	char password[255]; //field 5: password
} AUTH_REQUEST;
/*
*  sock5 response
*/
typedef struct _CONN_RESPONSE {// connect to remote
	char version; //field 1: SOCKS version number, 1 byte (must be 0x05 for this version)
	char resp;// field 2: status, 1 byte:
		// 0x00 = request granted
		// 0x01 = general failure
		// 0x02 = connection not allowed by ruleset
		// 0x03 = network unreachable
		// 0x04 = host unreachable
		// 0x05 = connection refused by destination host
		// 0x06 = TTL expired
		// 0x07 = command not supported / protocol error
		// 0x08 = address type not supported
	char reserved; // field 3: reserved, must be 0x00
	char address_type; // field 4: address type, 1 byte:
			// 0x01 = IPv4 address
			// 0x03 = Domain name
			// 0x04 = IPv6 address
	char dest_addr[255];//field 5: destination address of
			// 4 bytes for IPv4 address
			// 1 byte of name length followed by the name for domain name
			// 16 bytes for IPv6 address
	char port[2];//field 6: port number in a network byte order, 2 bytes

}CONN_RESPONSE;
/*
*  sock5 request
*/
typedef struct _CONN_REQUEST {// connect to remote
	char version;  // Socks version:0x04 or 0x05
	char cmd; // field 2: command code, 1 byte:
		// 0x01 = establish a TCP/IP stream connection
		// 0x02 = establish a TCP/IP port binding
		// 0x03 = associate a UDP port
	char reserved; // field 3: reserved, must be 0x00
	char address_type; // field 4: address type, 1 byte:
			// 0x01 = IPv4 address
			// 0x03 = Domain name
			// 0x04 = IPv6 address
	char dest_addr[255];//field 5: destination address of
			// 4 bytes for IPv4 address
			// 1 byte of name length followed by the name for domain name
			// 16 bytes for IPv6 address
	char port[2];//field 6: port number in a network byte order, 2 bytes
}CONN_REQUEST;
#endif
