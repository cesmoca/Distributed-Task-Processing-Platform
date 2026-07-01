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
#include <future>

#include <common/task.h>
#include <common/thread_safe_queue.h>
#include <worker/worker_pool.h>

namespace DTPP {

	class Scheduler {
	public:
		
		enum class Status {
			Pending,
			Running,
			Completed,
			Failed,
			Cancelled
		};

		struct TaskInfo {
			Task::Id id;
			Scheduler::Status status;
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
				case Scheduler::Status::Pending: statusStr = "Pending"; break;
				case Scheduler::Status::Running: statusStr = "Running"; break;
				case Scheduler::Status::Completed: statusStr = "Completed"; break;
				case Scheduler::Status::Failed: statusStr = "Failed"; break;
				case Scheduler::Status::Cancelled: statusStr = "Cancelled"; break;
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
				[this](Task::Id id, Task::Result&& result) { onTaskCompleted(id, std::move(result));  },
				[this](Task::Id id) { onTaskCancelled(id);  }) {}

		~Scheduler();

		void start();
		void cancelTasksAndWait();
		void finishTasksAndWait();

		Scheduler::Status getTaskStatus(Task::Id id);
		Scheduler::TaskInfo getTaskInfo(Task::Id id);

		template <typename Callable>
		std::optional<std::future<Scheduler::TaskInfo>> submitTask(Callable&& task);

	
	private:

		class InternalTaskInfo {
		public:
			InternalTaskInfo(const TaskInfo& taskInfo): taskInfo(taskInfo){}

			// Let's make this non copyable for starters, since it has a promise
			InternalTaskInfo(const InternalTaskInfo&) = delete;
			InternalTaskInfo operator=(const InternalTaskInfo&) = delete;

			InternalTaskInfo(InternalTaskInfo&&) = default;
			InternalTaskInfo& operator=(InternalTaskInfo&&) = default;

			TaskInfo taskInfo;
			std::optional<Task::Result> taskResult;
			std::promise<TaskInfo> promiseTaskInfo;
		};

		ThreadSafeQueue<Task> queue_;
		WorkerPool<Task> workerPool_;
		std::mutex tasksRegistryMutex_;
		std::unordered_map<Task::Id, InternalTaskInfo> tasksRegistry_;
		std::atomic<Task::Id> nextId_ = 0;
		bool acceptingNewTasks = true;

		void onTaskStarted(Task::Id id);
		void onTaskCompleted(Task::Id id, Task::Result&& result);
		void onTaskCancelled(Task::Id id);
	};

	// Template functions implementation
	template <typename Callable>
	std::optional<std::future<Scheduler::TaskInfo>> Scheduler::submitTask(Callable&& task) {
		if (!acceptingNewTasks) return std::nullopt;

		std::lock_guard lock(tasksRegistryMutex_);
		Task::Id taskId = nextId_;
		nextId_++;

		// Create the taskInfo and push it to the taskRegistry
		Scheduler::TaskInfo taskInfo{ taskId, Scheduler::Status::Pending };
		taskInfo.createdAt = Task::Timestamp(std::chrono::steady_clock::now());
		tasksRegistry_.emplace(taskId, std::move(InternalTaskInfo(taskInfo)));

		auto& internalTaskInfo = tasksRegistry_.at(taskId);

		queue_.push(
			std::make_unique<Task>(
				taskId,
				std::forward<Callable>(task)
			)
		);

		return internalTaskInfo.promiseTaskInfo.get_future();
	}

};
