unit_server: unit_server.c utils.c encrypt.c
		clang -pthread unit_server.c utils.c encrypt.c -o unit_server
unit_client: unit_client.c utils.c encrypt.c
		clang -pthread unit_client.c utils.c encrypt.c -o unit_client
.PHONY: clean

clean:
	rm -rf unit_client unit_server
