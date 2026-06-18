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
		
		Scheduler(int nWorkers): 
			workerPool_(queue_, 
				nWorkers,
				[this](Task::Id id) { onTaskStarted(id); },
				[this](Task::Id id, Task::Result result) { onTaskCompleted(id, result);  }) {}

		~Scheduler();

		void start();
		void stopAndWait();

		template <typename Callable>
		void submitTask(Callable&& task);
		Task::Status trackTask(Task::Id id);
	
	private:
		ThreadSafeQueue queue_;
		WorkerPool<ThreadSafeQueue> workerPool_;
		std::mutex tasksRegistryMutex_;
		std::unordered_map<Task::Id, Task::Info> tasksRegistry_;
		std::atomic<Task::Id> nextId_ = 0;

		void onTaskStarted(Task::Id id);
		void onTaskCompleted(Task::Id id, Task::Result result);
	};

};

#include <scheduler/scheduler_impl.h>