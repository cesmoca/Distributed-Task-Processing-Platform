#include <common/task.h>
#include <iostream>
#include <cstdio>
#include <format>

using namespace DTPP;

Task::Result Task::execute(CancelRequest& cancelRequest) const {
	// TODO: at some point we might need to check
	//  if(work_) before calling it, but for now we will
	//  it will always be set by the constructor, so we 
	//  can skip that check
	std::cout << std::format("[Task {}] Starting work...\n", id_);
	auto taskResult = work_(cancelRequest.requested);
	std::cout << std::format("[Task {}] Ended work with result: {}\n", id_, taskResult.toString());

	return taskResult;
}