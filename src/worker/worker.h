#pragma once
#include <thread>
#include <cstdint>
#include <functional>
#include <atomic>
#include <utility>

#include <common/task.h>
#include <common/thread_safe_queue.h>

namespace DTPP {

	template<typename Queue>
	class Worker {

	public:

		using Id = std::uint64_t;

		enum class StopMode{
			STOP_PROCESSING_TASKS,
			FINISH_ALL_TASKS_AND_STOP
		};

		Worker(Id id, 
			Queue& queue,
			std::function<void(Task::Id)> onTaskStarted,
			std::function<void(Task::Id, Task::Result&&)> onTaskCompleted
			) :	id_(id), queue_(queue), 
			onTaskStarted_(std::move(onTaskStarted)), 
			onTaskCompleted_(std::move(onTaskCompleted)) {}


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
		Queue& queue_;
		std::jthread thread_;
		std::atomic<bool> stopWhenQueueEmpty = false;
		const std::function<void(Task::Id)> onTaskStarted_;
		const std::function<void(Task::Id, Task::Result&&)> onTaskCompleted_;

		void run(std::stop_token stopToken);

	};
}

#include <worker/worker_impl.h>
