#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

#include <common/task.h>

namespace DTPP {
	class ThreadSafeQueue {

	public:

		ThreadSafeQueue() = default;

		// Deleting copy and moving, because it owns a mutex and 
		//  condition_variable. This object should not be
		//  changing ownership
		ThreadSafeQueue(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue(const ThreadSafeQueue&&) = delete;
		ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

		void push(Task task);

		Task waitAndPop();

		bool tryPop(Task& value);

		bool empty() const;

	private:

		mutable std::mutex mutex_;
		std::queue<Task> queue_;
		std::condition_variable conditionVar_;


	};
}