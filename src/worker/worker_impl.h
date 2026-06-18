#pragma once
#include <iostream>
#include <format>
#include <string>
#include <utils.h>

namespace DTPP {

	template<typename Queue>
	void Worker<Queue>::start() {
		std::cout << std::format("[Worker {} {}] Starting worker thread...\n", id_, static_cast<const void*>(this));

		thread_ = std::jthread{ [this](std::stop_token stopToken) {
			std::cout << std::format("[Worker {} {} {}] Started thread execution \n", id_, static_cast<const void*>(this), Utils::threadId());
			run(stopToken);
		} };

	}

	template<typename Queue>
	void Worker<Queue>::stopAndWait() {
		std::cout << std::format("[Worker {} {}] Requested stop and wait\n", id_, static_cast<const void*>(this));
		thread_.get_stop_source().request_stop();
		if(thread_.joinable()) thread_.join();
	}

	template<typename Queue>
	void Worker<Queue>::run(std::stop_token stopToken) {
		std::cout << std::format("[Worker {} {} {}] Running worker thread...\n", id_, static_cast<const void*>(this), Utils::threadId());

		while (!stopToken.stop_requested()) {
			auto task = queue_.waitAndPop(); // Wait for a task to be available in the queue

			if (task) {
				std::cout << std::format("[Worker {} {} {}] Executing task {}...\n", id_, static_cast<const void*>(this), Utils::threadId(), task->id());
				
				onTaskStarted_(task->id());

				auto result = task->execute();

				onTaskCompleted_(task->id(), result);
			}
			else {
				// Means we are stopping, so let's get out
				break;
			}
		}

		std::cout << std::format("[Worker {} {} {}] Worker finished\n", id_, static_cast<const void*>(this), Utils::threadId());

	}

	template <typename Queue>
	Worker<Queue>::~Worker() {
		std::cout << std::format("~[Worker {} {}]\n", id_, static_cast<const void*>(this));
		//std::cout << std::format("~[Worker {} {}] Worker requesting stop in destructor\n", id_, static_cast<const void*>(this));
		stopAndWait();
		//std::cout << std::format("~[Worker {} {}] Worker destroyed\n", id_, static_cast<const void*>(this));
	}


};