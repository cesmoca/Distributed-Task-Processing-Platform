#pragma once

#include <cstdint>

namespace DTPP {

	template<typename Queue>
	void WorkerPool<Queue>::start() {

		workers_.reserve(workerCount_);

		// Create and start the worker counts
		for (int i = 0; i < workerCount_; ++i) {
			int id = nextId_;
			nextId_++;
			workers_.emplace_back(id, queue_,
				onTaskStarted_, onTaskCompleted_);
		}

		for (auto& w : workers_) {
			w.start();
		}
	}

	template<typename Queue>
	void WorkerPool<Queue>::stopAndWait() {

		// Stop all the workers
		for (auto& worker : workers_) {
			worker.stopAndWait();
		}
	}

	template<typename Queue>
	WorkerPool<Queue>::~WorkerPool() {
		std::cout << std::format("~[WorkerPool]\n");
		stopAndWait();
	}
};