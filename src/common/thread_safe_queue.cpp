#include <common/thread_safe_queue.h>
#include <iostream>
#include <format>

using namespace DTPP;

void ThreadSafeQueue::push(std::unique_ptr<Task> task) {
	{
		std::lock_guard lock{ mutex_ };
		queue_.push(std::move(task));
	}

	conditionVar_.notify_one();
}

std::unique_ptr<Task> ThreadSafeQueue::waitAndPop() {
	std::unique_lock lock{ mutex_ };

	conditionVar_.wait(lock, [this]() {
		return !queue_.empty() || stopping_;
	});

	// Exit the wait because we are done, not because
	if (stopping_) return nullptr;

	auto task = std::move(queue_.front());
	queue_.pop();
	return task;

}

std::unique_ptr<Task> ThreadSafeQueue::tryPopOrNull() {
	std::lock_guard lock{ mutex_ };
	if (queue_.empty()) {
		return nullptr;
	}else {
		auto task = std::move(queue_.front());
		queue_.pop();
		return task;
	}
}

bool ThreadSafeQueue::empty() const {
	std::lock_guard lock{ mutex_ };
	return queue_.empty();
}

void ThreadSafeQueue::stop() {
	std::lock_guard lock{ mutex_ };
	stopping_ = true;
	conditionVar_.notify_all();
}

ThreadSafeQueue::~ThreadSafeQueue() {
	std::cout << std::format("~[ThreadSafeQueue]\n");
	stop();
}