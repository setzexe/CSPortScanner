# CSPortScanner

CSPortScanner (CyberSeverance Port Scanneer) is a fast, multi-threaded TCP port scanner based on C++ using POSIX sockets and non-blocking connections to scan large port ranges efficiently. It serves as a demonstration of network level programming and systems. 

## Features
- Multithreaded TCP scanning
- Configurable port range, worker counter, and timeout
- Open / closed / filtered / error classification for port status
- Scan duration reporting

## How It Works

This tool uses the standard **TCP Connect Scan** via ```connect()``` combined with non-blocking sockets.

- It initiates a standard TCP three way handshake to the target port via thread workers.
- Because sockets are set to non-blocking, the scanner does not freeze while waiting for slower network responses.
- ```select()```is used for custom timeouts. If a port is established within that timeout window, the port is classified as **open**. If it received a reset flag (ECONNREFUSED for example), it marks it as **closed.** If the connection times out or goes through error, it is marked as **filtered / unreachable**.

## Usage

```./portscanner <IP_address> <START_PORT> <END_PORT>  <THREADS> <TIMEOUT_MS>```

Port range must be 1 - 65535.

Thread count must be 1 - 20.

Timeout must be 1 - 500 ms.

## Build

```make``` compile

```make clean``` to revert a compilation.

## Example

```./portscanner 127.0.0.1 1 20000 50 500```

Target: 127.0.0.1  
Ports: 1-20000  
Threads: 50  
Timeout: 500 ms  

22/tcp OPEN  
631/tcp OPEN  

Scan complete! ^.^  
Open: 2  
Closed: 19998  
Filtered / Unreachable: 0  
Errors: 0  
Duration: 81 ms

## Current Limitations / Further Things to Improve
- TCP only
- IPv4 only

## Ethical Use

Only scan systems you have authorization to test, or systems you own!
