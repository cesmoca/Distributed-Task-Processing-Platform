#pragma once
#include <memory>
#include <common/task.h>
#include <utility>

namespace DTPP {

	template <typename Callable>
	void Scheduler::submitTask(Callable&& task) {
		Task::Id taskId = nextId_;
		nextId_++;

		// Create the taskInfo and push it to the taskRegistry
		Task::Info taskInfo{ taskId, Task::Status::Pending };

		queue_.push(
			std::make_unique<Task>(
				taskId,
				std::forward<Callable>(task)
			)
		);

	}

	void Scheduler::start() {
		workerPool_.start();
	}

	void Scheduler::stopAndWait() {
		queue_.stop();
		workerPool_.stop();
	}
}