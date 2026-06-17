#pragma once
#include <memory>
#include <atomic>

#include <common/thread_safe_queue.h>
#include <worker/worker_pool.h>

namespace DTPP {

	class Scheduler {
	public:
		
		Scheduler(int nWorkers) : queue_(ThreadSafeQueue{}), workerPool_(WorkerPool(queue_, nWorkers)) {}

		void start();
		void stopAndWait();

		template <typename Callable>
		void submitTask(Callable&& task);
		//Task::Status trackTask(Task::Id id);
		//Task::Info queryTask(Task::Id id);

		

	private:
		ThreadSafeQueue queue_;
		// pending tasks (still not popped, in queue_)
		// running tasks (retrieved by the worker)
		// completed tasks (the worker notified its result)
		WorkerPool<ThreadSafeQueue> workerPool_;
		std::atomic<Task::Id> nextId_ = 0;

	};

};

#include <scheduler/scheduler_impl.h>