#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>
#include <memory>

namespace DTPP {

	template <typename T>
	class ThreadSafeQueue {

	public:

		ThreadSafeQueue() = default;
		~ThreadSafeQueue();

		// Deleting copy and moving, because it owns a mutex and 
		//  condition_variable. This object should not be
		//  changing ownership
		ThreadSafeQueue(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue(ThreadSafeQueue&&) = delete;
		ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

		void push(std::unique_ptr<T> task);

		std::unique_ptr<T> waitAndPop();

		[[nodiscard]]
		std::unique_ptr<T> tryPopOrNull();

		bool empty() const;

		void stop();

	private:

		mutable std::mutex mutex_;
		std::queue<std::unique_ptr<T>> queue_;
		std::condition_variable conditionVar_;
		bool stopping_ = false;

	};
}

#include <common/thread_safe_queue_impl.h>