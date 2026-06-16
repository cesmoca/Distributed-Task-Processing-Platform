#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>

#include <common/task.h>
#include <fakes/fake_task.h>

class FakeQueue {

public:

	void push(std::unique_ptr<FakeTask> task) {
		{
			std::lock_guard lock{ mutex_ };
			queue.push_back(std::move(task));
		}
		conditionVar_.notify_one();
		pushCalled = true;

	}


	std::unique_ptr<FakeTask> waitAndPop() {
		std::unique_lock lock(mutex_);
		waitAndPopCalled = true;

		promise.set_value();

		conditionVar_.wait(lock, [this]() {
			return !queue.empty() || stopping_;
		});

		if (stopping_) return nullptr;
		
		auto value = std::move(queue.front());
		queue.pop_back();

		return value;
	}

	std::unique_ptr<FakeTask> tryPopOrNull() {
		tryPopOrNullCalled = true;
	}

	bool empty() {
		std::lock_guard lock{ mutex_ };
		emptyCalled = true;
		return queue.empty();
	}

	void stop() {
		{
			std::lock_guard lock{ mutex_ };
			stopping_ = true;
		}

		conditionVar_.notify_all();
		stopCalled = true;
	}

	void resetFake() {
		pushCalled = false;
		waitAndPopCalled = false;
		tryPopOrNullCalled = false;
		emptyCalled = false;
		stopCalled = false;
		stopping_ = false;
		queue.clear();
		promise = std::promise<void>{};
	}

	bool pushCalled = false;
	bool waitAndPopCalled = false;
	bool tryPopOrNullCalled = false;
	bool emptyCalled = false;
	bool stopCalled = false;

	std::vector<std::unique_ptr<FakeTask>> queue{};

	// Helps to synchronize with waitAndPop
	std::promise<void> promise;

private:
	std::condition_variable conditionVar_;
	mutable std::mutex mutex_;
	bool stopping_ = false;
};