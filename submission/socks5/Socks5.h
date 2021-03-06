/* *
 *  CS296 Honor Project
 */
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

#define AUTH_MODE 0x02

#define TIME_OUT 6000000

#define MIN -15



typedef enum _socks5_status {
	SOCKS5_ERROR_CLIENT = MIN,
	SOCKS5_ERROR_SOCKET,
	SOCKS5_ERROR_BIND,
	SOCKS5_ERROR_LISTEN,
	SOCKS5_ERROR_CONNECT,
	SOCKS5_ERROR_GETADDRINFO,
	SOCKS5_ERROR_ADDRTYPE,
	SOCKS5_ERROR_THREAD,
	SOCKS5_ERROR_SEND,
	SOCKS5_ERROR_RECV,
	SOCKS5_ERROR_RECV_CLIENT,
	SOCKS5_ERROR_RECV_SERVER,
	SOCKS5_ERROR_METHOD,
	SOCKS5_ERROR_AUTH,
	SOCKS5_ERROR_ACKCMD,
	SOCKS5_ERROR_USAGE,
	SOCKS5_ERROR_TARGET,
	SOCKS5_SUCCESS
} SOCKS5_STATUS;
