#pragma once
#include <common/thread_safe_queue.h>
#include <thread>
#include <cstdint>

namespace DTPP {

	class Worker {

	public:
		Worker(std::uint64_t id, ThreadSafeQueue& queue) : id_(id), queue_(queue) {}

		void start();

		void stop();

	private:
		std::uint64_t id_;
		ThreadSafeQueue& queue_;
		std::stop_token stopToken_;

		void run();

	};
}