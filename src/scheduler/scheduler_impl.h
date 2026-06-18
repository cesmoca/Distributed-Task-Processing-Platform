#pragma once
#include <memory>
#include <common/task.h>
#include <utility>
#include <stdexcept>
#include <format>

namespace DTPP {

	template <typename Callable>
	void Scheduler::submitTask(Callable&& task) {
		std::lock_guard lock(tasksRegistryMutex_);
		Task::Id taskId = nextId_;
		nextId_++;

		// Create the taskInfo and push it to the taskRegistry
		Task::Info taskInfo{ taskId, Task::Status::Pending };
		tasksRegistry_.emplace(taskId, taskInfo);

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
		workerPool_.stopAndWait();
	}

	Task::Status Scheduler::trackTask(Task::Id id) {
		std::lock_guard lock(tasksRegistryMutex_);
		auto it = tasksRegistry_.find(id);
		if (it == tasksRegistry_.end()) {
			throw std::out_of_range(std::format("Task with id {} not found", id));
		}

		return it->second.status;
	}

	void Scheduler::onTaskStarted(Task::Id id) {
		std::lock_guard lock(tasksRegistryMutex_);
		auto it = tasksRegistry_.find(id);
		if (it == tasksRegistry_.end()) {
			throw std::out_of_range(std::format("Task with id {} not found", id));
		}

		it->second.status = Task::Status::Running;
	};

	void Scheduler::onTaskCompleted(Task::Id id, Task::Result result) {
		
			std::lock_guard lock(tasksRegistryMutex_);
			auto it = tasksRegistry_.find(id);
			if (it == tasksRegistry_.end()) {
				throw std::out_of_range(std::format("Task with id {} not found", id));
			}

			it->second.status = result.success ? Task::Status::Completed : Task::Status::Failed;
		
	}

	Scheduler::~Scheduler() {
		std::cout << std::format("~[Scheduler]\n");
		stopAndWait();
	}
}