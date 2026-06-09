#pragma once

#include <mutex>
#include <queue>
#include <cstdio>
#include <iostream>
#include <condition_variable>

namespace DTPP {
	class ThreadSafeQueue {

	public:

		ThreadSafeQueue() = default;

		void push(int value);

		int waitAndPop();

		bool tryPop(int& value);

		bool empty() const;

	private:

		mutable std::mutex mutex_;
		std::queue<int> queue_;
		std::condition_variable conditionVar_;


	};
}