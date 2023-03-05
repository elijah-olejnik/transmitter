#pragma once
#include <exception>
#include <portaudio.h>
#include <lame/lame.h>
#include <shout/shout.h>
#include <boost/thread.hpp>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
boost::mutex io_mutex;
boost::mutex stream_mutex;
class TMException : public std::exception {
public:
	TMException(int paCode = 0, shout_t *str = nullptr, const char *msg = nullptr);
	char *what();
private:
	int code;
	shout_t *stream;
	char* message;
};
