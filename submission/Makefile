all: unit_server unit_client demo_proxy demo_client

unit_server: unit_server.c utils.c encrypt.c
		clang -pthread unit_server.c utils.c encrypt.c -o unit_server
unit_client: unit_client.c utils.c encrypt.c
		clang -pthread unit_client.c utils.c encrypt.c -o unit_client
demo_proxy: demo_proxy.c utils.h encrypt.h
		clang -pthread demo_proxy.c utils.h encrypt.h -o demo_proxy
demo_client: demo_client.c utils.h encrypt.h
		clang -pthread demo_client.c utils.h encrypt.h -o demo_client

server: utils.c
.PHONY: clean

clean:
	rm -rf unit_client unit_server picture_server
