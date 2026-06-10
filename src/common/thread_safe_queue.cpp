#include <common/thread_safe_queue.h>

using namespace DTPP;

void ThreadSafeQueue::push(Task task) {
	{
		std::lock_guard lock{ mutex_ };
		queue_.push(std::move(task));
	}

	conditionVar_.notify_one();
}

Task ThreadSafeQueue::waitAndPop() {
	std::unique_lock lock{ mutex_ };

	conditionVar_.wait(lock, [this]() {
		return !queue_.empty();
	});


	Task task = std::move(queue_.front());
	queue_.pop();
	return task;

}

bool ThreadSafeQueue::tryPop(Task& task) {
	std::lock_guard lock{ mutex_ };
	if (queue_.empty()) return false;
	else {
		task = std::move(queue_.front());
		queue_.pop();
		return true;
	}
}

bool ThreadSafeQueue::empty() const {
	std::lock_guard lock{ mutex_ };
	return queue_.empty();
}