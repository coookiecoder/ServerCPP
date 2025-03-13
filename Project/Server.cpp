#include "Server.hpp"

Server::Server(int port) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        throw std::runtime_error("[error] | unable to create socket");
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &this->server_socket_option, sizeof(this->server_socket_option))) {
        throw std::runtime_error("[error] | unable to set socket options");
    }

    std::cout << "[info]  | server socket open" << std::endl;

    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1) {
        throw std::runtime_error("[error] | unable to set socket options");
    }

    std::cout << "[info]  | server socket options set" << std::endl;

    std::memset(&this->addr, 0, sizeof(struct sockaddr));

    this->addr.sin_family = AF_INET;
    this->addr.sin_addr.s_addr = INADDR_ANY;
    this->addr.sin_port = htons(port);

    this->server_port = port;

    if (bind(server_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        throw std::runtime_error("[error] | unable to bind the socket");
    }

    std::cout << "[info]  | sever bound" << std::endl;

    if (listen(server_socket, 32) == -1) {
        throw std::runtime_error("[error] | unable to listen on socket");
    }

    std::cout << "[info]  | server listening" << std::endl;

    pollfd fds[1024] = {};

    fds[0].fd = server_socket;
    fds[0].events = POLLIN;
}

Server::~Server() {
    std::cout << "[info]  | closing socket" << std::endl;
    close(server_socket);
}



void Server::start() {
    std::cout << "[info]  | starting server on port : " << this->server_port << std::endl;
    this->ServerThread = std::thread([this] () {this->server_loop();});
}

void Server::stop() {
    std::cout << "[info]  | stopping server" << std::endl;
    this->running = false;
    this->ServerThread.join();
}

void Server::server_loop() const {
    std::cout << "[info]  | starting server thread" << std::endl;

    int ready_fds = 0;
    int n_fds = 1;

    pollfd poll_fds[1024];

    poll_fds[0].fd = this->server_socket;
    poll_fds[0].events = POLLIN;

    while (this->running) {
        ready_fds = poll(poll_fds, n_fds, 1000);

		if (!ready_fds)
			continue;

		std::cout << "[debug] | " << ready_fds << " fds are ready" << std::endl;

        for (int current_fd = 0; current_fd < n_fds && ready_fds > 0; ++current_fd) {
            if (poll_fds[current_fd].revents & POLLIN) {
                if (poll_fds[current_fd].fd == server_socket) {
                    this->handle_new_connection(poll_fds, n_fds);
                } else {
                    this->handle_old_connection(poll_fds[current_fd].fd, poll_fds, n_fds, current_fd);
                }
                ready_fds--;
            }
        }
    }
}

void Server::handle_new_connection(pollfd poll_fds[1024], int &n_fds) const {
    int new_client = accept(server_socket, nullptr, nullptr);

    if (new_client == -1) {
        std::cerr << "[warn]  | unable to accept new client" << std::endl;
    } else {
        std::cout << "[info]  | new client connected fd : " << new_client << std::endl;
        fcntl(new_client, F_SETFL, O_NONBLOCK);
        poll_fds[n_fds].fd = new_client;
        poll_fds[n_fds].events = POLLIN;
        n_fds++;
    }
}

void Server::handle_old_connection(int client_fd, pollfd poll_fds[1024], int &n_fds, int &current_fd) const {
    char buffer[513] = {};
    ssize_t response_size = recv(client_fd, buffer, sizeof(buffer), 0);

    if (response_size > 0) {
		std::stringstream result;
        result << buffer;
		while (response_size == 513) {
			response_size = recv(client_fd, buffer, sizeof(buffer), 0);
			result << buffer;
		}
		std::cout << "[info]  | message from " << client_fd << " size : " << result.str().size() << " byte" << std::endl;
        write(client_fd, "ping\n", 5);
    } else {
        std::cout << "[info]  | removing client fd : " << client_fd << std::endl;
        for (int i = current_fd; i < n_fds - 1; ++i) {
            poll_fds[i] = poll_fds[i + 1];
        }
        n_fds--;
        poll_fds[n_fds].fd = -1;
        poll_fds[n_fds].events = 0;
        poll_fds[n_fds].revents = 0;
        current_fd--; // Stay on the same index to check the next item after removal
        close(client_fd);
    }
}
