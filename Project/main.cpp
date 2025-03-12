#include <iostream>
#include <sstream>
#include <vector>
#include <csignal>

#include "Server.hpp"

Server **servers;

int number_of_server;

void stop(int code) {
	std::cout << "[info]  | stopping all servers" << std::endl;
	for (int current_server = 0; current_server < number_of_server; ++current_server)
		servers[current_server]->stop();
	exit(0);
}

int main(int argc, char **argv) {
	servers = new Server*[argc - 1];

	for (int current_server = 1; current_server < argc; ++current_server) {
		int port;
		std::stringstream(argv[current_server]) >> port;

		if (port <= 1024 && port != 0)
			std::cout << "[info]  | need root permission for port <= 1024, (will still try to use the port)" << std::endl;

		if (port == 0)
			std::cout << "[indo]  | assigning a random port since the port 0 is specified" << std::endl;

		servers[current_server] = new Server(port);
	}

	for (int current_server = 1; current_server < argc; ++current_server) {
		servers[current_server]->start();
	}

	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	while (true) {}

	return 0;
}
