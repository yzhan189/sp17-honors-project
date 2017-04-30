/* *
 *  CS296 Honor Project
 */
// #include "utils.h"
// #include "encrypt.h"

#include "Socks5.h"
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
#include <netinet/in.h>
#include <arpa/inet.h>


static volatile int serverSocket;

//network
void close_program(int signal);
void close_server_connection();
int connect_to_server(const char* host, const char* port, const char *username,
	const char *password, const char *target_domain, const char* target_port);


int main(int argc, char **argv) {

    if (argc != 7) {
        fprintf(stderr, "Usage: %s <host> <port> <username> <password> <target_domain> <target_port>\n",
                argv[0]);
        return SOCKS5_ERROR_USAGE;
    }


    signal(SIGINT, close_program);
    serverSocket = connect_to_server(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

	if(serverSocket == SOCKS5_ERROR_AUTH) {
		fprintf(stderr, "UNAUTHORIZED USER!\n");
		return serverSocket;
	}

	char *buffer = "GET / HTTP/1.0\r\n\r\n";
	printf("SENDING: %s", buffer);
	printf("===\n");
	write(serverSocket, buffer, strlen(buffer));


	char resp[1000];
	int len = read(serverSocket, resp, 999);
	resp[len] = '\0';
	printf("%s\n", resp);
    // char resp[1000];
    // int len = 0;
    // while ((len = read (serverSocket, resp, 999)) > 0) {
	//     resp[len] = '\0';
	//     printf("%s\n", resp);
    // }
    return SOCKS5_SUCCESS;
}

/**
 * Shuts down connection with 'serverSocket'.
 * Called by close_program upon SIGINT.
 */
void close_server_connection() {
	// Your code here
	shutdown(serverSocket, SHUT_RDWR);
	close(serverSocket);
}

/*
 * Signal handler used to close this client program.
 */
void close_program(int signal) {
    if (signal == SIGINT) {
        close_server_connection();
    }
}
/**
 * Sets up a connection to a proxy server and returns
 * the file descriptor associated with the connection.
 *
 * host - Server to connect to.
 * port - Port to connect to server on.
 *
 * Returns integer of valid file descriptor, or error status on failure.
 */
int connect_to_server(const char* host, const char* port, const char *username,
                      const char *password, const char *target_domain, const char* target_port) {

	/**
	 *	create socket
	 */
	int s;
	struct addrinfo server_addr, *result;
	bzero(&server_addr, sizeof(struct addrinfo));

	server_addr.ai_family = AF_INET; /* IPv4 only */
	server_addr.ai_socktype = SOCK_STREAM; /* TCP */

	s = getaddrinfo(host, port, &server_addr, &result);

	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return SOCKS5_ERROR_GETADDRINFO;
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	int ret = connect(serverSocket, result -> ai_addr, result -> ai_addrlen);
	if(ret == -1){
		perror("connect to proxy");
		return SOCKS5_ERROR_CONNECT;
	}

	fprintf(stderr, "connect successfully\n");
	/**
	 *	connect successfully
	 *	start verify the SOCKS version
	 */
	int i;
	/**
	*  GET AUTH METHOD
	*/
	char buffer[BUFF_SIZE] = {0};
	char *pen = buffer;
	*pen ++ = SOCKS5_VERSION;// version
	*pen ++ = 2;// support method count
	*pen ++ = AUTH_MODE;// auth mode
	*pen ++ = 0x00;//

	fprintf(stderr, "%u\n", (unsigned)buffer[0]);
	fprintf(stderr, "%u\n", (unsigned)buffer[1]);
	fprintf(stderr, "%u\n", (unsigned)buffer[2]);
	fprintf(stderr, "%u\n", (unsigned)buffer[3]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[4]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[5]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[6]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[7]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[8]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[9]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[10]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[11]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[12]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[13]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[14]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[15]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[16]);
	// fprintf(stderr, "%u\n", (unsigned)buffer[17]);
	// fprintf(stderr, "%zu: %s\n", (size_t)(pen - buffer), buffer);

	send(serverSocket, buffer, pen - buffer, 0);

	fprintf(stderr, "%s reached line %d in %s\n", __FILE__,__LINE__,__FUNCTION__);
	fprintf(stderr, "METHOD: recv %zu byte from server\n", strlen(buffer));

	recv(serverSocket, buffer, 2, 0);

	fprintf(stderr, "resp: %s\n", buffer);
	if((unsigned int)*(buffer + 1) == 0xFF) {
		perror("version not support");
		exit(-1);
	}
	else if((unsigned int)*(buffer + 1) == 0x02) {
		//version supported
		fprintf(stderr, "version supported, sending username & password for AUTH\n");

		// send usrname
		pen = buffer;
		*pen ++ = 0x01;
		// pen ++;
		size_t username_len = strlen(username);
		*pen ++ = username_len;
		strcpy(pen, username);
		fprintf(stderr, "username: %s\n",pen );
		pen += username_len;
		//send password

		size_t password_len = strlen(password);
		*pen ++ = password_len;
		strcpy(pen, password);
		fprintf(stderr, "password: %s\n",pen);
		pen += password_len;


		fprintf(stderr, "AUTH: send %u byte from server = [%s]\n", (unsigned int)(pen - buffer), buffer);

		send(serverSocket, buffer, pen - buffer, 0);
		fprintf(stderr, "%s reached line %d in %s\n", __FILE__,__LINE__,__FUNCTION__);
		fprintf(stderr, "AUTH: recv %zu byte from server\n", strlen(buffer));
		recv(serverSocket, buffer, 2, 0);

		if((unsigned int)*(buffer + 1) != 0) {
			perror("Authentication");
			return SOCKS5_ERROR_AUTH;
		}
		fprintf(stderr, "CLIENT AUTH PASS!\n");
		// Authentication pass
		// send connect cmd
		pen = buffer;
		*pen ++ = SOCKS5_VERSION;

		*pen ++ = 1;

		*pen ++ = 0;

		*pen ++ = SOCKS5_DOMAIN; //DOMAIN

		/**
		* get the address of target destination
		*/
		*pen ++ = strlen(target_domain);
		strncpy(pen, target_domain, strlen(target_domain));
		pen += strlen(target_domain);
		uint32_t port =  htons(atoi(target_port));
		fprintf(stderr, "port: %d\n", port);
		strncpy(pen, &port, sizeof(port));
		pen += strlen(target_port);

		fprintf(stderr, "i want to go: %s[%s] %d\n", target_domain, inet_ntoa(*(struct in_addr*)&target_domain), port);

		fprintf(stderr, "send %u byte to server\n", (unsigned int)(pen - buffer));
		send(serverSocket, buffer, pen - buffer, 0);
		recv(serverSocket, buffer, 4, 0);
		fprintf(stderr, "recv %zu byte from server\n", strlen(buffer));

		if((unsigned int)*(buffer + 1) == 0x00) {
			fprintf(stderr, "Success!" );
		}
		else {
			perror("client connect to remote");
			return SOCKS5_ERROR_CLIENT;
		}

	}
	return ret;
}
