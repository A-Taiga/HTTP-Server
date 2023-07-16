//
// Created by Anthony Polka on 7/15/23.
//

#ifndef HTTP_SERVER_3_CACHE_HPP
#define HTTP_SERVER_3_CACHE_HPP
//#define DEBUG

#include <list>
#include <unordered_map>
#include <utility>
#include <string>
#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#ifdef DEBUG

#define RED     "\x1B[31;1m"
#define GREEN   "\x1B[32;1m"
#define RESET   "\x1B[0m"

inline void end(auto& start)
{
	std::cout << "==================" << std::endl;
	std::cout << GREEN << "TIME: " << (std::chrono::steady_clock::now() - start).count() << RESET<< std::endl;
}
#endif

template<std::size_t maxSize>
class LRU
{
	private:
	typedef std::list<std::pair<std::string,std::string>> list;
	list l;
	std::unordered_map<std::string,list::iterator> map;
	std::string requestFile(std::string& file);

	public:
	LRU();
	std::string cache(std::string key);

};




#endif // HTTP_SERVER_3_CACHE_HPP
