#pragma once

#include <vector>

#include <common/thread_safe_queue.h>
#include <worker/worker.h>

namespace DTPP {

	template <typename Queue>
	
	class WorkerPool{

	public:
		WorkerPool(Queue& queue) : queue_(queue) {}

		void start(int workerCount);
		
		void stop();

	private:
		Queue& queue_;
		std::vector<Worker<Queue>> workers_;

	};

}

#include <worker/worker_pool_impl.h>