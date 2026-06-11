#pragma once
#include <common/thread_safe_queue.h>
#include <thread>
#include <cstdint>

namespace DTPP {

	class Worker {

	public:
		Worker(std::uint64_t id, ThreadSafeQueue& queue) : id_(id), queue_(queue) {}

		// This class owns a std::jthread, so it has to be
		//  movable, but not copyable
		Worker() = delete;
		Worker(const Worker&) = delete;
		Worker& operator=(const Worker&) = delete;

		Worker(Worker&&) = default;
		Worker& operator=(Worker&&) = default;

		void start();

		void stop();

	private:
		std::uint64_t id_;
		ThreadSafeQueue& queue_;
		std::jthread thread_;

		void run(std::stop_token stopToken);

	};
}