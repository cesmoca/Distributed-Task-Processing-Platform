#pragma once
#include <thread>
#include <cstdint>
#include <functional>
#include <atomic>
#include <optional>
#include <utility>

#include <common/task.h>
#include <common/thread_safe_queue.h>

namespace DTPP {

	template<typename T>
	class Worker {

	public:

		using Id = std::uint64_t;

		enum class StopMode{
			CANCEL_TASKS_AND_STOP,
			FINISH_ALL_TASKS_AND_STOP
		};

		Worker(Id id, 
			ThreadSafeQueue<T>& queue,
			std::function<void(Task::Id)> onTaskStarted,
			std::function<void(Task::Id, Task::Result&&)> onTaskCompleted,
			std::function<void(Task::Id)> onTaskCancelled
			) :	id_(id), queue_(queue), 
			onTaskStarted_(std::move(onTaskStarted)), 
			onTaskCompleted_(std::move(onTaskCompleted)),
			onTaskCancelled_(std::move(onTaskCancelled)){}


		// This class owns a std::jthread, so it has to be
		//  movable, but not copyable
		Worker() = delete;
		Worker(const Worker&) = delete;
		Worker& operator=(const Worker&) = delete;

		Worker(Worker&&) = delete;
		Worker& operator=(Worker&&) = delete;

		~Worker();

		void start();

		void stop(StopMode stopMode);
		void waitUntilFinished();

	private:
		Id id_;
		ThreadSafeQueue<T>& queue_;
		std::jthread thread_;
		std::optional<StopMode> stopMode_;
		Task::CancelRequest cancelRequest_;
		std::mutex mutex_;
		const std::function<void(Task::Id)> onTaskStarted_;
		const std::function<void(Task::Id, Task::Result&&)> onTaskCompleted_;
		const std::function<void(Task::Id)> onTaskCancelled_;

		void run(std::stop_token stopToken);
		void performTask(std::unique_ptr<T>& task);

	};
}

#include <worker/worker_impl.h>
