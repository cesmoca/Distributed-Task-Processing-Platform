#pragma once

#include <vector>
#include <atomic>
#include <functional>
#include <memory>

#include <common/task.h>
#include <common/thread_safe_queue.h>
#include <worker/worker.h>

namespace DTPP {

	template <typename T>
	class WorkerPool{

	public:

		WorkerPool(ThreadSafeQueue<T>& queue, 
			int workerCount, 
			std::function<void(Task::Id)> onTaskStarted,
			std::function<void(Task::Id, Task::Result&&)> onTaskCompleted,
			std::function<void(Task::Id)> onTaskCancelled
		) : queue_(queue), 
			workerCount_(workerCount), 
			nextId_(0),
			onTaskStarted_(std::move(onTaskStarted)),
			onTaskCompleted_(std::move(onTaskCompleted)),
			onTaskCancelled_(std::move(onTaskCancelled)) {}

		~WorkerPool();

		void start();
		
		void stopAndWait(Worker<T>::StopMode stopMode);

	private:
		ThreadSafeQueue<T>& queue_;
		const int workerCount_;
		std::atomic<typename Worker<T>::Id> nextId_;
		std::vector<std::unique_ptr<Worker<T>>> workers_;
		const std::function<void(Task::Id)> onTaskStarted_;
		const std::function<void(Task::Id, Task::Result&&)> onTaskCompleted_;
		const std::function<void(Task::Id)> onTaskCancelled_;
	};

}

#include <worker/worker_pool_impl.h>
