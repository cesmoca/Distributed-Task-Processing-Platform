#pragma once

#include <vector>

#include <common/thread_safe_queue.h>
#include <worker/worker.h>

namespace DTPP {
	
	class WorkerPool{
	public:

		WorkerPool(ThreadSafeQueue& queue) : queue_(queue) {}

		void start(int workerCount);
		
		void stop();

	private:
		ThreadSafeQueue& queue_;
		std::vector<Worker> workers_;

	};

}