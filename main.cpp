#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cout << "Usage: " << argv[0] << " <IP_address> <PORT>";
		return 1;
	}

	string target_ip = argv[1];
	int target_port = stoi(argv[2]);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		cerr << "Failed to create socket!" << endl;
	}

	sockaddr_in target_address{};
	target_address.sin_family = AF_INET;
	target_address.sin_port = htons(target_port);

        if (inet_pton(AF_INET, target_ip.c_str(), &target_address.sin_addr) <= 0) {
       		cerr << "Invalid IP address." << endl;
        	close(sock);
        	return 1;
    	}

    	int result = connect(sock,
		reinterpret_cast<sockaddr*>(&target_address),
        	sizeof(target_address)
    	);

    	if (result == 0) {
        	cout << "Port " << target_port << " is OPEN." << endl;
    	} else {
        	cout << "Port " << target_port << " is CLOSED / unreachable." << endl;
    	}

    	close(sock);

	return 0;
}

