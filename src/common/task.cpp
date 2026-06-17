#include <common/task.h>
#include <iostream>
#include <cstdio>
#include <format>

using namespace DTPP;

//Task::Task(std::uint64_t id, Type type, const std::string& name) : Task(id, name) {
//	assert(false && "Not implemented yet");
//}

Task::Result Task::execute() const {
	// TODO: at some point we might need to check
	//  if(work_) before calling it, but for now we will
	//  it will always be set by the constructor, so we 
	//  can skip that check
	std::cout << std::format("[Task {}] Starting work...\n", info_.id);
	auto taskResult = work_();
	std::cout << std::format("[Task {}] Ended work with result: {}\n", info_.id, taskResult.toString());

	return taskResult;
}