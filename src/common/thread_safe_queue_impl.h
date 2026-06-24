#pragma once

#include <iostream>
#include <format>
#include <mutex>
#include <utility>

using namespace DTPP;

template <typename T>
void ThreadSafeQueue<T>::push(std::unique_ptr<T> task) {
	{
		std::lock_guard lock{ mutex_ };
		if (stopping_) return; // We are not accepting new tasks

		queue_.push(std::move(task));
	}

	conditionVar_.notify_one();
}

template <typename T>
std::unique_ptr<T> ThreadSafeQueue<T>::waitAndPop() {
	std::unique_lock lock{ mutex_ };

	conditionVar_.wait(lock, [this]() {
		return !queue_.empty() // A task arrived
			|| stopping_; // We are not blocking workers
	});

	// Stopping means that we are shutting down, but
	//  it is up to the workers to decide what we
	//  are doing with the remaining tasks
	if (queue_.empty()) return nullptr;

	auto task = std::move(queue_.front());
	queue_.pop();
	return task;

}

template <typename T>
std::unique_ptr<T> ThreadSafeQueue<T>::tryPopOrNull() {
	std::lock_guard lock{ mutex_ };
	if (queue_.empty()) {
		return nullptr;
	}else {
		auto task = std::move(queue_.front());
		queue_.pop();
		return task;
	}
}

template <typename T>
bool ThreadSafeQueue<T>::empty() const {
	std::lock_guard lock{ mutex_ };
	return queue_.empty();
}

template <typename T>
void ThreadSafeQueue<T>::stop() {
	std::lock_guard lock{ mutex_ };
	stopping_ = true;
	conditionVar_.notify_all();
}

template <typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() {
	//std::cout << std::format("~[ThreadSafeQueue]\n");
	stop();
}
