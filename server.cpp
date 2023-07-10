//
// Created by anthony on 7/6/23.
//
#include "server.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void* get_in_addr(sockaddr* sa)
{
	if (sa->sa_family == AF_INET)
		return &(reinterpret_cast<sockaddr_in*>(sa)->sin_addr);
	return &(reinterpret_cast<sockaddr_in6*>(sa)->sin6_addr);
}

std::string get_user_address(sockaddr_storage& client)
{
	std::string address(INET6_ADDRSTRLEN, '\0');
	inet_ntop(
	    client.ss_family,
	    get_in_addr(reinterpret_cast<sockaddr*>(&client)),
	    address.data(),
	    address.size()
	);
	return address;
}
Server::Server(const char* port)
{
	int       s, status;
	const int option      = 1;
	addrinfo* temp_info   = {nullptr};
	addrinfo* server_info = {nullptr};
	addrinfo  hints       = {};

	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_protocol = 0;

	status = getaddrinfo(nullptr, port, &hints, &temp_info);
	if (status != 0)
		std::cerr << "Server::Server getaddrinfo(): " << gai_strerror(status) << std::endl;
	for (server_info = temp_info; server_info != nullptr; server_info = server_info->ai_next)
	{
		listen_socket =
		    socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
		if (listen_socket == -1)
			ERROR("socket()");
		if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
			ERROR("setsockopt()");
		if (bind(listen_socket, server_info->ai_addr, server_info->ai_addrlen) == -1)
		{
			close(listen_socket);
			std::cerr << "server bind" << strerror(errno) << std::endl;
			continue;
		}
		break;
	}
	freeaddrinfo(temp_info);
	if (server_info == nullptr)
		ERROR("failed to bind");

#ifdef __linux__
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		ERROR(__FILE__, __LINE__, __PRETTY_FUNCTION__, "epoll_creat1 failed");

	epoll_event ev = {};
	ev.events      = EPOLLIN;
	ev.data.fd     = listen_socket;
	s              = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_socket, &ev);
	if (s == -1)
		ERROR(__FILE__, __LINE__, __PRETTY_FUNCTION__, "epoll_ctl failed");
#endif
#ifdef __MACH__
		/// kqueue code not implemented yet
#endif
}
void Server::listen(int queue_size)
{
	std::cout << GREEN << "server started" RESET << std::endl;
	if (::listen(listen_socket, queue_size) == -1)
		ERROR("listen()");
}
void Server::accept()
{
#ifdef __linux__
	epoll_event event = {};
#endif
	std::string      user_address = {};
	sockaddr_storage connection   = {};
	socklen_t        sin_size     = 0;
	int              client_fd;

	sin_size  = sizeof(connection);
	client_fd = ::accept(listen_socket, reinterpret_cast<sockaddr*>(&connection), &sin_size);
	if (client_fd == -1)
		ERROR("accept()");
	user_address = get_user_address(connection);

#ifdef __linux__
	epoll_event ev = {};
	ev.events      = EPOLLIN;
	ev.data.fd     = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
		ERROR(__FILE__, __LINE__, __PRETTY_FUNCTION__, "epoll_ctl failed");
#endif
}

#ifdef __linux__
void Server::run()
{
	int event_count = 0;
	listen(20);
	while (true)
	{
		event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
		for (int i = 0; i < event_count; i++)
		{
			if (events[i].data.fd == listen_socket)
			{
				accept();
			}
			else
			{
				char b[] = "HTTP/1.1 200 OK\n"
				           "Connection: close\n"
				           "Content-Type:text/html\n"
				           "Server: Anthony's HTTP server\n"
				           "Content-Length: 17\n\n"
				           "<h1>Hello Bitches</h1>";

				char buffer[1024] = {};
				::recv(events[i].data.fd, buffer, sizeof(buffer), 0);
				std::cout << buffer << std::endl;
				::send(events[i].data.fd, b, sizeof(b), 0);

				events[i].events = EPOLLONESHOT;
				int evfd         = events[i].data.fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, evfd, &events[i]);
				close(events[i].data.fd);
			}
		}
	}
}
#endif

#ifdef __MACH__
void Server::run() {}
#endif
Server::~Server()
{
	close(listen_socket);
	std::cout << RED << "server ended" << RESET << std::endl;
}
