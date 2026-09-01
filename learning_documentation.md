# Learning Documentation

This markdown file exists to document different code or techniques used in the project for learning purposes, particular stuff I am not used to or have not done prior. This is my first project working with C++ inside of a terminal as opposed to an IDE. 

## New Syntax

```int main(int argc, char* argv[])``` is how we take input. Bash's counterpart is $1, $2, and so on. argv[1], argv[2], etc.

For development and convenience purposes, we can utilize C++'s ability to create classes and functions prior to main[]. We do this to declare our blocks for the actual port scanning ability itself, ```PortState``` and ```scanPort```.

We use the ```try(...) catch(...)``` block to verify that input is properly formatted. The previous ```(arcg < 6)``` check only checks the argument count.

## Sockets

```int sock = socket(AF_INET, SOCK_STREAM, 0)``` creates a **socket** under IPv4 (AR_INET) within a proper TCP connection (SOCK_STREAM, 0). A socket is a software endpoint that allows two programs to send and receive data across a network.

An important concept to know is that we are working with non-blocking. Blocking means that threads could sit and wait for several moments for a single port to MAYBE respond. Using ```fcntl(sock, F_SETFL, flags | O_NONBLOCK)``` makes the socket non-blocking, allowing us to have our timer.

```
sockaddr_in target_address{};
target_address.sin_family = AF_INET;
target_address.sin_port = htons(target_port);
```
This block of code creates the target address for the socket. We do {} to make memory blank; this is important so our target destination is accurate. AF_INET tells the network we are communicating via IPv4. 
Our computer cannot just put a port like '80' and a connection is made. htons stores that address in proper network byte order.

Below this is an if block with ```inet_pton(AF_INET, target_ip.c_str(), &target_address.sin_addr)```. This converts human readable text, our IP in our case, into a format that Linux can understand.

At the end of the socket life (when the thread finishes its search) we close the socket so we do not use up memory. ```close(sock)```

## Threads (Workers)

We utilize threads, which creates their own independent workers, through the ```auto worker = [&]() {...}``` function. These simply just ask for a connection from the socket side. Linux's networking system does the TCP packets, connection state, replies, etc. But these workers ask for the connection.

```mutex output_mutex``` is important because based on how thread workers line up to give an output, they could get outputs tangled up. This mutex prevents that.

The other very important bit is the fact that we have a shared port counter via ```atomic<int> next_port{start_port};```. All workers look at this number. Due to it being atomic, once the number increments for one person, it flows naturally with everyone else. No one accidentally duplicates work.

## Connectivity & Networking

```connect(...)``` is when we do actual networking behavior. Now, TCP attempts the TCP handshake. Due to how Linux flags work, we have to use certain codes and linguo as opposed to just stating a port is opened or closed. We need to check if the scan even worked via the ```EINPROGRESS``` flag, and then take that result later and run it through flags like ```ETIMEDOUT```, ```EHOSTUNREACH```, etc. 

```select(...)``` waits for a response. On our end, we just create the socket and wait. Linux does all the networking mechanics. When select() gets a response / gets an answer from the thread, ```getsockopt()``` analyzes this response and turns it into a port state; if ```ETIMEDOUT```, it might be FILTERED / UNREACHABLE. If we get 0 as the select() result, its OPEN.



