#pragma once

#include <vector>
#include <atomic>
#include <functional>
#include <memory>

#include <common/task.h>
#include <common/thread_safe_queue.h>
#include <worker/worker.h>

namespace DTPP {

	template <typename Queue>
	class WorkerPool{

	public:

		WorkerPool(Queue& queue, 
			int workerCount, 
			std::function<void(Task::Id)> onTaskStarted,
			std::function<void(Task::Id, Task::Result&&)> onTaskCompleted
		) : queue_(queue), workerCount_(workerCount), nextId_(0),
			onTaskStarted_(onTaskStarted), onTaskCompleted_(onTaskCompleted){}

		~WorkerPool();

		void start();
		
		void stopAndWait(Worker<Queue>::StopMode stopMode);

	private:
		Queue& queue_;
		const int workerCount_;
		std::atomic<typename Worker<Queue>::Id> nextId_;
		std::vector<std::unique_ptr<Worker<Queue>>> workers_;
		const std::function<void(Task::Id)> onTaskStarted_;
		const std::function<void(Task::Id, Task::Result&&)> onTaskCompleted_;
	};

}

#include <worker/worker_pool_impl.h>
