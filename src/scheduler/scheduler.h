#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <stdexcept>

#include <common/thread_safe_queue.h>
#include <worker/worker_pool.h>

namespace DTPP {

	class Scheduler {
	public:
		
		Scheduler(int nWorkers): workerPool_(queue_, nWorkers) {}

		void start();
		void stopAndWait();

		template <typename Callable>
		void submitTask(Callable&& task);
		//Task::Status trackTask(Task::Id id);
		//Task::Info queryTask(Task::Id id);

	
	private:
		ThreadSafeQueue queue_;
		WorkerPool<ThreadSafeQueue> workerPool_;
		std::mutex tasksRegistryMutex_;
		std::unordered_map<Task::Id, Task::Info> tasksRegistry_;
		std::atomic<Task::Id> nextId_ = 0;

	};

};

#include <scheduler/scheduler_impl.h>