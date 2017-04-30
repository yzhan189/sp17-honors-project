all: socks5_server socks5_client

socks5_server: socks5_server.c
		clang -lpthread -o socks5_server -O2 socks5_server.c
socks5_client: socks5_client.c
		clang -lpthread -o socks5_client -O2 socks5_client.c
.PHONY: clean

clean:
	rm -rf socks5_client socks5_server
