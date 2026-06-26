#pragma once
#include <scheduler/scheduler.h>

#include <memory>
#include <utility>
#include <stdexcept>
#include <format>
#include <iostream>
#include <mutex>
#include <chrono>

#include <common/task.h>

using namespace DTPP;

void Scheduler::start() {
	workerPool_.start();
}

void Scheduler::cancelTasksAndWait() {
	queue_.stop(); // Cancel all queued tasks
	workerPool_.stopAndWait(Worker<ThreadSafeQueue<Task>>::StopMode::STOP_PROCESSING_TASKS);
}

void Scheduler::finishTasksAndWait() {
	queue_.stop(); // We finish the ones that are already queued
	workerPool_.stopAndWait(Worker<ThreadSafeQueue<Task>>::StopMode::FINISH_ALL_TASKS_AND_STOP);
}

Task::Status Scheduler::getTaskStatus(Task::Id id) {
	std::lock_guard lock(tasksRegistryMutex_);
	auto it = tasksRegistry_.find(id);
	if (it == tasksRegistry_.end()) {
		throw std::out_of_range(std::format("Task with id {} not found", id));
	}

	return it->second.taskInfo.status;
}

[[nodiscard]]
Scheduler::TaskInfo Scheduler::getTaskInfo(Task::Id id) {
	std::lock_guard lock(tasksRegistryMutex_);
	auto it = tasksRegistry_.find(id);
	if (it == tasksRegistry_.end()) {
		throw std::out_of_range(std::format("Task with id {} not found", id));
	}

	return it->second.taskInfo;
}


void Scheduler::onTaskStarted(Task::Id id) {
	std::lock_guard lock(tasksRegistryMutex_);
	auto it = tasksRegistry_.find(id);
	if (it == tasksRegistry_.end()) {
		throw std::out_of_range(std::format("Task with id {} not found", id));
	}

	it->second.taskInfo.startedAt = Task::Timestamp(std::chrono::steady_clock::now());
	it->second.taskInfo.status = Task::Status::Running;
};

void Scheduler::onTaskCompleted(Task::Id id, Task::Result&& result) {

	std::lock_guard lock(tasksRegistryMutex_);
	auto taskInternalInfo = tasksRegistry_.find(id);
	if (taskInternalInfo == tasksRegistry_.end()) {
		throw std::out_of_range(std::format("Task with id {} not found", id));
	}

	taskInternalInfo->second.taskInfo.finishedAt = Task::Timestamp(std::chrono::steady_clock::now());
	taskInternalInfo->second.taskInfo.status = result.success ? Task::Status::Completed : Task::Status::Failed;
	taskInternalInfo->second.taskInfo.result = std::move(result);	
	taskInternalInfo->second.promiseTaskInfo.set_value(taskInternalInfo->second.taskInfo);
}

Scheduler::~Scheduler() {
	//std::cout << std::format("~[Scheduler]\n");
	// If the scheduler is being destroyed, let's close as fast as possible
	cancelTasksAndWait();
}
