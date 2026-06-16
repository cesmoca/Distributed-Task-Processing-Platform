#pragma once

#include <vector>

#include <common/thread_safe_queue.h>
#include <worker/worker.h>

namespace DTPP {

	template <typename Queue>
	
	class WorkerPool{

	public:
		WorkerPool(Queue& queue, int workerCount) : queue_(queue), workerCount_(workerCount) {}

		void start();
		
		void stop();

	private:
		Queue& queue_;
		int workerCount_;
		std::vector<Worker<Queue>> workers_;
		
	};

}

#include <worker/worker_pool_impl.h>