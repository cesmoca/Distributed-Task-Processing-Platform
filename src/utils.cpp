#include <utils.h>

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <format>

std::string Utils::threadId(std::thread::id threadId) {
	std::ostringstream oss;
	oss << threadId;

	return oss.str();
}

std::string Utils::threadId() {
	return threadId(std::this_thread::get_id());
}
