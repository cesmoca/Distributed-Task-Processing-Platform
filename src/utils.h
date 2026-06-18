#pragma once
#include <string>
#include <thread>


namespace Utils {

	std::string threadId(std::thread::id threadId);

	std::string threadId();

}