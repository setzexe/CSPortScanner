# Learning Documentation

This markdown file exists to document different code or techniques used in the project for learning purposes, particular stuff I am not used to or have not done prior. This is my first project working with C++ inside of a terminal as opposed to an IDE. 
___

```int main(int argc, char* argv[])``` is how we take input. Bash's counterpart is $1, $2, and so on. argv[1], argv[2], etc.

```int sock = socket(AF_INET, SOCK_STREAM, 0)``` creates a **socket** under IPv4 (AR_INET) within a proper TCP connection (SOCK_STREAM, 0). A socket is a software endpoint that allows two programs to send and receive data across a network.

```
sockaddr_in target_address{};
target_address.sin_family = AF_INET;
target_address.sin_port = htons(target_port);
```
This block of code creates the target address for the socket. We do {} to make memory blank; this is important so our target destination is accurate. AF_INET tells the network we are communicating via IPv4. 
Our computer cannot just put a port like '80' and a connection is made. htons turns it into a language that the computer / network can understand. 

# Might just finish this when I finish the general project?
