#pragma once

#include <vector>
#include <atomic>

#include <common/thread_safe_queue.h>
#include <worker/worker.h>

namespace DTPP {

	template <typename Queue>
	class WorkerPool{

	public:
		WorkerPool(Queue& queue, int workerCount) : queue_(queue), workerCount_(workerCount), nextId_(0) {}

		void start();
		
		void stop();

	private:
		Queue& queue_;
		const int workerCount_;
		std::atomic<typename Worker<Queue>::Id> nextId_;
		std::vector<Worker<Queue>> workers_;
		
	};

}

#include <worker/worker_pool_impl.h>