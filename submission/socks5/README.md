## useful termianl cmd
1. curl --proxy socks5://admin:12345@localhost:1997 www.google.com --> fail authentication
1.2 curl --proxy socks5://admin:123@localhost:1997 www.google.com --> pass authentication

2. netstat -tlnp


## TODO:
1. Error handling
2. Client connect timeout
3. Use getaddrinfo to replace addrin
