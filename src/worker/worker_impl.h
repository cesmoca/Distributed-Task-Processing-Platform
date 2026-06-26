#pragma once
#include <iostream>
#include <format>
#include <string>
#include <stdexcept>
#include <thread>
#include <utility>
#include <utils.h>

namespace DTPP {

	template<typename T>
	void Worker<T>::start() {
		std::cout << std::format("[Worker {} {}] Starting worker thread...\n", id_, static_cast<const void*>(this));

		thread_ = std::jthread{ [this](std::stop_token stopToken) {
			//std::cout << std::format("[Worker {} {} {}] Started thread execution \n", id_, static_cast<const void*>(this), Utils::threadId());
			run(stopToken);
		} };

	}

	template<typename T>
	void Worker<T>::stop(StopMode stopMode) {
		thread_.request_stop();
		stopMode_ = stopMode;
	}

	template<typename T>
	void Worker<T>::waitUntilFinished() {
		if (thread_.joinable())
			thread_.join();
	}

	template<typename T>
	void Worker<T>::run(std::stop_token stopToken) {
		std::cout << std::format("[Worker {} {} {}] Running worker thread...\n", id_, static_cast<const void*>(this), Utils::threadId());
		bool running = true;

		while (running) {

			// If we are shutting down, this API does not block
			std::unique_ptr<T> task = std::move(queue_.waitAndPop()); // Wait for a task to be available in the queue

			// If task is nullptr, means that the queue is empty, and stopping.
			if (task) {
				// What to do when the queue is NOT empty
				//  - if stop_requested:
				//     - if mode is CANCEL_TASKS, mark it as cancel and continue to the next one
				//     - if mode is FINISH_TASKS, process normally and on to the next one, until queue empty
				//  - if not, process the task normally. It will wait even with empty queue
				if (stopToken.stop_requested()) {
					switch (stopMode_.value()) {
					case StopMode::CANCEL_TASKS_AND_STOP: onTaskCancelled_(task->id());  break;
					case StopMode::FINISH_ALL_TASKS_AND_STOP: performTask(task);  break;
					default: throw std::logic_error("Unsupported StopMode");
					}
				}
				else {
					performTask(task);
				}

			}
			else {
				// What to do when the queue is empty and stopping (return nullptr)
				//  - if stop_requested, stop worker execution
				//  - if not, let's continue until stop is requested
				//    technically, if we got here, means that the queue is
				//    empty and stopping, so the workers could stop working,
				//    but let's follow the protocol and wait for the stpo request
				if (stopToken.stop_requested()) running = false;

			}

		}

		// Stop when queue empty is ALWAYS

		//while (!stopToken.stop_requested()) {
		//	
		//	// So this logic decides
		//	//  - When we are still running tasks normally
		//	//  - When we are running tasks normally until de queue is empty, then stop
		//	//  - When we are not doing any more tasks, and mark them as cancelled,
		//	//		all until the queue is empty again.
		//	if (stopWhenQueueEmpty && queue_.empty()) break;

		//	auto task = queue_.waitAndPop(); // Wait for a task to be available in the queue

		//	if (stopToken.stop_requested()) break;

		//	if (task) {
		//		std::cout << std::format("[Worker {} {} {}] Executing task {}...\n", id_, static_cast<const void*>(this), Utils::threadId(), task->id());
		//		
		//		onTaskStarted_(task->id());

		//		auto result = task->execute();

		//		onTaskCompleted_(task->id(), std::move(result));
		//	}

		//}

		std::cout << std::format("[Worker {} {} {}] Worker finished\n", id_, static_cast<const void*>(this), Utils::threadId());

	}

	template <typename T>
	void Worker<T>::performTask(std::unique_ptr<T>& task) {
		std::cout << std::format("[Worker {} {} {}] Executing task {}...\n", id_, static_cast<const void*>(this), Utils::threadId(), task->id());

		onTaskStarted_(task->id());

		auto result = task->execute();

		onTaskCompleted_(task->id(), std::move(result));
	}

	template <typename T>
	Worker<T>::~Worker() {
		//std::cout << std::format("~[Worker]\n");
		stop(StopMode::CANCEL_TASKS_AND_STOP);
		waitUntilFinished();
	}

};
