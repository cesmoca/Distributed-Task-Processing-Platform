#pragma once
#include <iostream>
#include <format>
#include <string>
#include <utility>
#include <utils.h>

namespace DTPP {

	template<typename Queue>
	void Worker<Queue>::start() {
		std::cout << std::format("[Worker {} {}] Starting worker thread...\n", id_, static_cast<const void*>(this));

		thread_ = std::jthread{ [this](std::stop_token stopToken) {
			//std::cout << std::format("[Worker {} {} {}] Started thread execution \n", id_, static_cast<const void*>(this), Utils::threadId());
			run(stopToken);
		} };

	}

	template<typename Queue>
	void Worker<Queue>::stopAndWait(StopMode stopMode) {
		switch (stopMode) {
			case StopMode::STOP_PROCESSING_TASKS: { thread_.get_stop_source().request_stop(); break; }
			case StopMode::FINISH_ALL_TASKS_AND_STOP: { stopWhenQueueEmpty = true;  break; }
			default: throw std::logic_error("Unsupported stop mode");
		}
		
		if(thread_.joinable()) 
			thread_.join();
	}

	template<typename Queue>
	void Worker<Queue>::run(std::stop_token stopToken) {
		std::cout << std::format("[Worker {} {} {}] Running worker thread...\n", id_, static_cast<const void*>(this), Utils::threadId());

		while (!stopToken.stop_requested()) {
			
			if (stopWhenQueueEmpty && queue_.empty()) break;

			auto task = queue_.waitAndPop(); // Wait for a task to be available in the queue

			if (stopToken.stop_requested()) break;

			if (task) {
				std::cout << std::format("[Worker {} {} {}] Executing task {}...\n", id_, static_cast<const void*>(this), Utils::threadId(), task->id());
				
				onTaskStarted_(task->id());

				auto result = task->execute();

				onTaskCompleted_(task->id(), std::move(result));
			}

		}

		std::cout << std::format("[Worker {} {} {}] Worker finished\n", id_, static_cast<const void*>(this), Utils::threadId());

	}

	template <typename Queue>
	Worker<Queue>::~Worker() {
		//std::cout << std::format("~[Worker]\n");
		stopAndWait(StopMode::STOP_PROCESSING_TASKS);
	}

};
