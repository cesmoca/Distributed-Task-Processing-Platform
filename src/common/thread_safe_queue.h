#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

#include <common/task.h>

namespace DTPP {
	class ThreadSafeQueue {

	public:

		ThreadSafeQueue() = default;

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