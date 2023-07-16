//
// Created by Anthony Polka on 7/15/23.
//

#include "cache.hpp"

template<std::size_t Size>
LRU<Size>::LRU(){}

template<std::size_t Size>
std::string LRU<Size>::requestFile(std::string& file)
{
	std::ifstream f(file);
	std::stringstream ss;
	std::string line = {};
	while(std::getline(f,line))
	{
		ss << line;
		ss << '\n';
	}
	return ss.str();
}


template<std::size_t Size>
std::string LRU<Size>::cache(std::string fileName)
{
#ifdef DEBUG
	auto start = std::chrono::steady_clock::now();
#endif

	if(l.empty()) /* initialize cache if empty */
	{
		l.push_front(std::make_pair(fileName,requestFile(fileName)));
		map[fileName] = l.begin();
	}
	else
	{
		if(map.find(fileName) != map.end()) /* if in cache move to front */
		{
			l.splice(l.begin(),l,map[fileName]);
		}
		else
		{
			if(l.size() >= Size) /* capacity reached pop back*/
			{
				map.erase(l.back().first);
				l.pop_back();
			}

			l.push_front(std::make_pair(fileName,requestFile(fileName)));
			map[fileName] = l.begin();
		}
	}
#ifdef DEBUG
	end(start);
#endif
	return l.begin()->second;
}
