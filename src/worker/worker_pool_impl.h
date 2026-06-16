#pragma once

#include <cstdint>

namespace DTPP {

	template<typename Queue>
	void WorkerPool<Queue>::start() {

		// Create and start the worker counts
		for (int i = 0; i < workerCount_; ++i) {
			workers_.emplace_back(static_cast<std::uint64_t>(i), queue_);
		}

		for (auto& w : workers_) {
			w.start();
		}
	}

	template<typename Queue>
	void WorkerPool<Queue>::stop() {

		// Stop all the workers
		for (auto& worker : workers_) {
			worker.requestStop();
			worker.join();
		}

	}
};