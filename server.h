//
// Created by anthony on 7/6/23.
//

#ifndef UNTITLED_SERVER_H
#define UNTITLED_SERVER_H

#ifdef __linux__
#include <sys/epoll.h>
#endif
#ifdef __MACH__
#include <sys/event.h>
#include <sys/time.h>
#endif

#include <vector>
#include "cache.cpp"
#define CLEAR   "\e[2J\e[3J\e[H"
#define BLACK   "\x1B[30;1m"
#define RED     "\x1B[31;1m"
#define GREEN   "\x1B[32;1m"
#define YELLOW  "\x1B[33;1m"
#define BLUE    "\x1B[34;1m"
#define MAGENTA "\x1B[35;1m"
#define CYAN    "\x1B[36;1m"
#define WHITE   "\x1B[37;1m"
#define RESET   "\x1B[0m"

#define B_BLACK   "\x1B[40;1m"
#define B_RED     "\x1B[41;1m"
#define B_GREEN   "\x1B[42;1m"
#define B_YELLOW  "\x1B[43;1m"
#define B_BLUE    "\x1B[44;1m"
#define B_MAGENTA "\x1B[45;1m"
#define B_CYAN    "\x1B[46;1m"
#define B_WHITE   "\x1B[47;1m"

#define MAX_EVENTS 100

#define ERROR(msg)                                                                             \
	{                                                                                          \
		std::cout << B_RED << WHITE << __FILE__ << ":" << __LINE__ << RESET << ":"             \
		          << __PRETTY_FUNCTION__ << ":" << RED << "errno:" << RESET << strerror(errno) \
		          << ":" << RED << msg << RESET << std::endl;                                  \
		std::exit(EXIT_FAILURE);                                                               \
	}
class Server
{
	private:
	LRU<10> c;
	int  epoll_fd;
	void listen(int queue_size);
	void accept();
/*
 *  https://suchprogramming.com/epoll-in-3-easy-steps/
 */
#ifdef __linux__
	epoll_event events[MAX_EVENTS];
#endif
#ifdef __MACH__
	int kq;
	int event_num;
	timespec timeout;
	struct kevent change_list;
	std::vector<struct kevent> event_list;
#endif
	public:
	int listen_socket;
	explicit Server(const char* port);
	~Server();
	void run();
};

#endif // UNTITLED_SERVER_H
