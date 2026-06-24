#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <stdexcept>
#include <utility>
#include <optional>
#include <string>
#include <format>
#include <chrono>

#include <common/task.h>
#include <common/thread_safe_queue.h>
#include <worker/worker_pool.h>

namespace DTPP {

	class Scheduler {
	public:
		
		struct TaskInfo {
			Task::Id id;
			Task::Status status;
			std::optional<Task::Result> result;

			Task::Timestamp createdAt;
			Task::Timestamp startedAt;
			Task::Timestamp finishedAt;

			Task::DurationMs getDurationMs() const {
				if (startedAt.has_value() && finishedAt.has_value()) {
					return std::optional{ 
						std::chrono::duration_cast<std::chrono::milliseconds>(*finishedAt - *startedAt)
					};
				}

				return Task::DurationMs{};
			}

			std::string toString() const {
				std::string statusStr;
				switch (status) {
				case Task::Status::Pending: statusStr = "Pending"; break;
				case Task::Status::Running: statusStr = "Running"; break;
				case Task::Status::Completed: statusStr = "Completed"; break;
				case Task::Status::Failed: statusStr = "Failed"; break;
				default: throw std::logic_error("Not all Task::Result implemented in toString");
				}

				std::string resultStr = "No result";
				if (result.has_value()) resultStr = std::format("Result {}", (*result).toString());

				std::chrono::milliseconds durationMs(0);
				auto durationOpt = getDurationMs();
				if (durationOpt.has_value()) durationMs = *getDurationMs();

				return std::format("[Task {} - {} ms] Status: {}. {}", id, durationMs, statusStr, resultStr);
			}
		};

		Scheduler(int nWorkers): 
			workerPool_(queue_, 
				nWorkers,
				[this](Task::Id id) { onTaskStarted(id); },
				[this](Task::Id id, Task::Result&& result) { onTaskCompleted(id, std::move(result));  }) {}

		~Scheduler();

		void start();
		void cancelTasksAndWait();
		void finishTasksAndWait();

		Task::Status getTaskStatus(Task::Id id);
		Scheduler::TaskInfo getTaskInfo(Task::Id id);

		template <typename Callable>
		void submitTask(Callable&& task);

	
	private:
		ThreadSafeQueue queue_;
		WorkerPool<ThreadSafeQueue> workerPool_;
		std::mutex tasksRegistryMutex_;
		std::unordered_map<Task::Id, Scheduler::TaskInfo> tasksRegistry_;
		std::atomic<Task::Id> nextId_ = 0;
		bool acceptingNewTasks = true;

		void onTaskStarted(Task::Id id);
		void onTaskCompleted(Task::Id id, Task::Result&& result);
	};

	// Template functions implementation
	template <typename Callable>
	void Scheduler::submitTask(Callable&& task) {
		if (!acceptingNewTasks) return;

		std::lock_guard lock(tasksRegistryMutex_);
		Task::Id taskId = nextId_;
		nextId_++;

		// Create the taskInfo and push it to the taskRegistry
		Scheduler::TaskInfo taskInfo{ taskId, Task::Status::Pending };
		taskInfo.createdAt = Task::Timestamp(std::chrono::steady_clock::now());
		tasksRegistry_.emplace(taskId, taskInfo);

		queue_.push(
			std::make_unique<Task>(
				taskId,
				std::forward<Callable>(task)
			)
		);
	}

};

