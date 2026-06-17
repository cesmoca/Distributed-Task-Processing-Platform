#pragma once
#include <memory>
#include <common/task.h>
#include <utility>

namespace DTPP {

	template <typename Callable>
	void Scheduler::submitTask(Callable&& task) {
		int id = nextId_;
		nextId_++;
		queue_.push(
			std::make_unique<Task>(
				id,
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