#pragma once
#include <scheduler/scheduler.h>

#include <memory>
#include <utility>
#include <stdexcept>
#include <format>
#include <iostream>
#include <mutex>

#include <common/task.h>

using namespace DTPP;

void Scheduler::start() {
	workerPool_.start();
}

void Scheduler::stopAndWait() {
	queue_.stop();
	workerPool_.stopAndWait();
}

Task::Status Scheduler::getTaskStatus(Task::Id id) {
	std::lock_guard lock(tasksRegistryMutex_);
	auto it = tasksRegistry_.find(id);
	if (it == tasksRegistry_.end()) {
		throw std::out_of_range(std::format("Task with id {} not found", id));
	}

	return it->second.status;
}

Scheduler::TaskInfo Scheduler::getTaskInfo(Task::Id id) {
	std::lock_guard lock(tasksRegistryMutex_);
	auto it = tasksRegistry_.find(id);
	if (it == tasksRegistry_.end()) {
		throw std::out_of_range(std::format("Task with id {} not found", id));
	}

	return it->second;
}


void Scheduler::onTaskStarted(Task::Id id) {
	std::lock_guard lock(tasksRegistryMutex_);
	auto it = tasksRegistry_.find(id);
	if (it == tasksRegistry_.end()) {
		throw std::out_of_range(std::format("Task with id {} not found", id));
	}

	it->second.status = Task::Status::Running;
};

void Scheduler::onTaskCompleted(Task::Id id, Task::Result&& result) {

	std::lock_guard lock(tasksRegistryMutex_);
	auto it = tasksRegistry_.find(id);
	if (it == tasksRegistry_.end()) {
		throw std::out_of_range(std::format("Task with id {} not found", id));
	}

	it->second.status = result.success ? Task::Status::Completed : Task::Status::Failed;
	it->second.result = std::move(result);
}

Scheduler::~Scheduler() {
	//std::cout << std::format("~[Scheduler]\n");
	stopAndWait();
}
