## TODO:
1. Error handling
2. Client connect timeout
3. Use getaddrinfo to replace addrin
4. Code clean up

## Demo idea
### 1. Basic connection status
use netstat -tlnp before and after running the server and prove there is a new tunnel establied by the proxy

### 2. Hide ip address
Use "checkmyip before and after connecting to the proxy, proving that ip is changed.
Prereq: Need to running proxy on VM, otherwise the ip addr is not affected.

Alter: 
use terminal to find local ip instead of checkmyip

### 3. Authentication
1. Use curl command to request a website with correct password and username and pipe the result into a html file, proving the website get the request successfully by opening it on web browser
2. Use curl cmd with wrong password, and show error msg for SOCKS_ERROR_AUTH

Alter:
1. Use Firefoxy with correct username and password to visit website, and prove it works fine.
2. Use it again with wrong password, and prove it doesn't work
### 4. Block website
Prereq: VM
#### 1. Block website
sudo /etc/hosts 
add two lines:
127.0.0.1 illinois.edu
127.0.0.1 www.illinois.edu
#### 2

## useful termianl cmd
1. curl --proxy socks5://admin:12345@localhost:1997 www.google.com --> fail authentication
1.2 curl --proxy socks5://admin:123@localhost:1997 www.google.com --> pass authentication

2. netstat -tlnp
