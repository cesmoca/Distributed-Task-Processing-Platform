#pragma once
#include <memory>

#include <common/thread_safe_queue.h>
#include <worker/worker_pool.h>

namespace DTPP {

	class Scheduler {
	public:
		
		Scheduler(int nWorkers) : queue_(ThreadSafeQueue{}), workerPool_(WorkerPool(queue_, nWorkers)) {}

		template <typename Callable>
		void submitTask(Callable&& task);

		void start();
		void stopAndWait();

	private:
		ThreadSafeQueue queue_;
		WorkerPool<ThreadSafeQueue> workerPool_;
		int nextId_ = 0;

	};


};

#include <scheduler/scheduler_impl.h>