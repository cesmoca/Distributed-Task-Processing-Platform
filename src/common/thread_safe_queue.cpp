#include <common/thread_safe_queue.h>

void ThreadSafeQueue::push(int value) {
	{
		std::lock_guard lock{ mutex_ };
		queue_.push(value);
	}

	conditionVar_.notify_one();
}

int ThreadSafeQueue::waitAndPop() {
	std::unique_lock lock{ mutex_ };

	if (queue_.empty()) {
		conditionVar_.wait(lock, [&] {
			return !queue_.empty();
		});

	}
	else {
		int value = queue_.front();
		queue_.pop();
		return value;
	}

}

bool ThreadSafeQueue::tryPop(int& value) {
	std::lock_guard lock{ mutex_ };
	if (queue_.empty()) return false;
	else {
		value = std::move(queue_.front());
		queue_.pop();
		return true;
	}
}

bool ThreadSafeQueue::empty() const {
	std::lock_guard lock{ mutex_ };
	return queue_.empty();
}