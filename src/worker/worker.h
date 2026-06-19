#pragma once
#include <thread>
#include <cstdint>
#include <functional>

#include <common/thread_safe_queue.h>

namespace DTPP {

	template<typename Queue>
	class Worker {

	public:

		using Id = std::uint64_t;

		Worker(Id id, 
			Queue& queue,
			std::function<void(Task::Id)> onTaskStarted,
			std::function<void(Task::Id, Task::Result&&)> onTaskCompleted
			) :	id_(id), queue_(queue), onTaskStarted_(onTaskStarted), onTaskCompleted_(onTaskCompleted) {}


		// This class owns a std::jthread, so it has to be
		//  movable, but not copyable
		Worker() = delete;
		Worker(const Worker&) = delete;
		Worker& operator=(const Worker&) = delete;

		Worker(Worker&&) = default;
		Worker& operator=(Worker&&) = default;

		~Worker();

		void start();

		void stopAndWait();

	private:
		Id id_;
		Queue& queue_;
		std::jthread thread_;
		const std::function<void(Task::Id)> onTaskStarted_;
		const std::function<void(Task::Id, Task::Result&&)> onTaskCompleted_;

		void run(std::stop_token stopToken);

	};
}

#include <worker/worker_impl.h>