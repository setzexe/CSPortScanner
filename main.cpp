#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <chrono>

using namespace std;

enum class PortState {
		OPEN,
		CLOSED,
    	FILTERED,
    	ERROR
};

PortState scanPort(const string& target_ip, int target_port, int timeout_ms) {
    	int sock = socket(AF_INET, SOCK_STREAM, 0);

		if (sock == -1) {
        	return PortState::ERROR;
    	}

    	int flags = fcntl(sock, F_GETFL, 0);

    	if (flags == -1) {
        	close(sock);
        	return PortState::ERROR;
    	}

    	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        	close(sock);
        	return PortState::ERROR;
    	}

    	sockaddr_in target_address{};
    	target_address.sin_family = AF_INET;
    	target_address.sin_port = htons(target_port);

    	if (inet_pton(AF_INET, target_ip.c_str(), &target_address.sin_addr) <= 0) {
        	close(sock);
        	return PortState::ERROR;
    	}

    	int result = connect(sock, reinterpret_cast<sockaddr*>(&target_address), sizeof(target_address));

    	if (result == 0) {
        	close(sock);
        	return PortState::OPEN;
    	}

    	if (errno != EINPROGRESS) {
        	int saved_errno = errno;
        	close(sock);
        	if (saved_errno == ECONNREFUSED) {
           		return PortState::CLOSED;
        	}
        	return PortState::ERROR;
    	}

    	fd_set write_fds;
    	FD_ZERO(&write_fds);
    	FD_SET(sock, &write_fds);

    	timeval timeout{};
    	timeout.tv_sec = timeout_ms / 1000;
    	timeout.tv_usec = (timeout_ms % 1000) * 1000;

    	result = select(sock + 1, nullptr, &write_fds, nullptr, &timeout);

    	if (result == 0) {
        	close(sock);
        	return PortState::FILTERED;
    	}

    	if (result == -1) {
        	close(sock);
        	return PortState::ERROR;
    	}

    	int socket_error = 0;
    	socklen_t error_length = sizeof(socket_error);

    	if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &socket_error, &error_length) == -1) {
        	close(sock);
        	return PortState::ERROR;
    	}

    	close(sock);

    	if (socket_error == 0) {
        	return PortState::OPEN;
    	}

    	if (socket_error == ECONNREFUSED) {
        	return PortState::CLOSED;
   	}

    	if (socket_error == ETIMEDOUT || socket_error == EHOSTUNREACH || socket_error == ENETUNREACH) {
        	return PortState::FILTERED;
    	}

    	return PortState::ERROR;
}

int main(int argc, char* argv[]) {
	if (argc < 6) {
        	cout << "Usage: " << argv[0] << " <IP_address> <START_PORT> <END_PORT>  <THREADS> <TIMEOUT_MS>" << endl;
        	return 1;
    	}

    	string target_ip = argv[1];
    	int start_port;
    	int end_port;
    	int thread_count;
    	int timeout_ms;

    	try {
        	start_port = stoi(argv[2]);
        	end_port = stoi(argv[3]);
        	thread_count = stoi(argv[4]);
        	timeout_ms = stoi(argv[5]);
    	}
    	catch (...) {
        	cerr << "Unable to take in input!" << endl;
        	return 1;
    	}

    	if (start_port < 1 || end_port > 65535 || start_port > end_port) {
       		cerr << "Invalid port range!" << endl;
        	return 1;
    	}

    	if (thread_count < 1 || thread_count > 500) {
        	cerr << "Thread count must be between 1 and 500!" << endl;
        	return 1;
    	}

    	if (timeout_ms < 1) {
        	cerr << "Timeout must be greater than 0 ms!" << endl;
        	return 1;
    	}

    	atomic<int> next_port{start_port};
    	atomic<int> open_count{0};
    	atomic<int> closed_count{0};
    	atomic<int> filtered_count{0};
    	atomic<int> error_count{0};

    	mutex output_mutex;
    	cout << "Target: " << target_ip << endl;
    	cout << "Ports: " << start_port << "-" << end_port << endl;
    	cout << "Threads: " << thread_count << endl;
    	cout << "Timeout: " << timeout_ms << " ms" << endl;
    	cout << endl;

    	auto start_time = chrono::steady_clock::now();
    	auto worker = [&]() {
        	while (true) {
            		int port = next_port.fetch_add(1);

            		if (port > end_port) {
                		break;
            		}

            		PortState state = scanPort(target_ip, port, timeout_ms);

            		if (state == PortState::OPEN) {
                		open_count++;
						lock_guard<mutex> lock(output_mutex);
                		cout << port << "/tcp OPEN" << endl;
            		} else if (state == PortState::CLOSED) {
                		closed_count++;
            		} else if (state == PortState::FILTERED) {
                		filtered_count++;
            		} else {
                		error_count++;
            		}
        	}
	};

	vector<thread> workers;

    	for (int i = 0; i < thread_count; i++) {
        	workers.emplace_back(worker);
    	}

    	for (thread& t : workers) {
        	t.join();
    	}

    	auto end_time = chrono::steady_clock::now();
    	auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    	cout << endl;
    	cout << "Scan complete! ^.^" << endl;
    	cout << "Open: " << open_count << endl;
    	cout << "Closed: " << closed_count << endl;
    	cout << "Filtered / Unreachable: " << filtered_count << endl;
    	cout << "Errors: " << error_count << endl;
    	cout << "Duration: " << duration.count() << " ms" << endl;

    	return 0;
}
