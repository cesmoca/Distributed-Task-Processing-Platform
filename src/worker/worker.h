#pragma once
#include <common/thread_safe_queue.h>
#include <thread>
#include <cstdint>

namespace DTPP {

	template<typename Queue>
	class Worker {

	public:

		using Id = std::uint64_t;

		Worker(Id id, Queue& queue) : id_(id), queue_(queue) {}

		// This class owns a std::jthread, so it has to be
		//  movable, but not copyable
		Worker() = delete;
		Worker(const Worker&) = delete;
		Worker& operator=(const Worker&) = delete;

		Worker(Worker&&) = default;
		Worker& operator=(Worker&&) = default;

		void start();
		void join();

		void requestStop();

	private:
		Id id_;
		Queue& queue_;
		std::jthread thread_;

		void run(std::stop_token stopToken);

	};
}

#include <worker/worker_impl.h>