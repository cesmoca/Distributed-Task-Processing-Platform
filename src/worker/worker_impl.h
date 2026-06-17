#pragma once
#include<iostream>
#include<format>

namespace DTPP {

	template<typename Queue>
	void Worker<Queue>::start() {
		std::cout << std::format("[Worker {}] Starting worker thread...\n", id_);

		thread_ = std::jthread{ [this](std::stop_token stopToken) {
			run(stopToken);
		} };

	}

	template<typename Queue>
	void Worker<Queue>::requestStop() {
		std::cout << std::format("[Worker {}] Stopping worker thread...\n", id_);

		thread_.get_stop_source().request_stop();
	}
	template<typename Queue>
	void Worker<Queue>::join() {
		thread_.join();
		std::cout << std::format("[Worker {}] Stopped.\n", id_);
	}

	template<typename Queue>
	void Worker<Queue>::run(std::stop_token stopToken) {
		std::cout << std::format("[Worker {}] Running worker thread...\n", id_);

		while (!stopToken.stop_requested()) {
			auto task = queue_.waitAndPop(); // Wait for a task to be available in the queue

			if (task) {
				std::cout << std::format("[Worker {}] Executing task {}...\n", id_, task->id());
				
				// TODO Here we move the tasks to running
				// Notify the scheduler somehow?

				auto result = task->execute();
				// TODO Here we move the task to completed or failed
				// Notify the scheduler somehow?
			}
			else {
				// Means we are stopping, so let's get out
				break;
			}
		}

		std::cout << std::format("[Worker {}] Worker finished\n", id_);

	}

};