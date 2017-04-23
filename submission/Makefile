all: unit_server unit_client picture_server

unit_server: unit_server.c utils.c encrypt.c
		clang -pthread unit_server.c utils.c encrypt.c -o unit_server
unit_client: unit_client.c utils.c encrypt.c
		clang -pthread unit_client.c utils.c encrypt.c -o unit_client
picture_server: server.c
		clang -pthread server.c -o server		
server: utils.c
.PHONY: clean

clean:
	rm -rf unit_client unit_server picture_server
