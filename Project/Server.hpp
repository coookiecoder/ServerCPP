#pragma once

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <csignal>
#include <thread>
#include <sstream>

#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>

class Server {
    private:
        int server_port;
        int server_socket = 0;
        int server_socket_option = 0;
        sockaddr_in addr = {};

        std::thread ServerThread;
        std::atomic<bool> running = true;

    public:
        explicit Server(int port);
        ~Server();
        void start();
        void stop();

    private:
        void server_loop() const;
        void handle_new_connection(pollfd poll_fds[1024], int &n_fds) const;
        void handle_old_connection(int client_fd, pollfd poll_fds[1024], int &n_fds, int &current_fd) const;
};