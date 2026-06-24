#pragma once

#include <cstdint>
#include <format>
#include <iostream>
#include <utility>

namespace DTPP {

	template<typename Queue>
	void WorkerPool<Queue>::start() {

		workers_.reserve(workerCount_);

		// Create and start the worker counts
		for (int i = 0; i < workerCount_; ++i) {
			typename Worker<Queue>::Id id = nextId_;
			nextId_++;
			workers_.emplace_back(
				std::make_unique<Worker<Queue>>(
					id, queue_, onTaskStarted_, onTaskCompleted_));
		}

		for (auto& w : workers_) {
			w->start();
		}
	}

	template<typename Queue>
	void WorkerPool<Queue>::stopAndWait(Worker<Queue>::StopMode stopMode) {

		for (auto& worker : workers_) worker->stopAndWait(stopMode);
	}

	template<typename Queue>
	WorkerPool<Queue>::~WorkerPool() {
		//std::cout << std::format("~[WorkerPool]\n");
		// In the destructor, we cancel all tasks and join the workers
		stopAndWait(Worker<Queue>::StopMode::STOP_PROCESSING_TASKS);
	}
};
