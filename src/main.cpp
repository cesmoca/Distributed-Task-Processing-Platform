#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdio>
#include <random>
#include <utility>
#include <format>
#include <memory>

#include <common/thread_safe_queue.h>
#include <common/task.h>
#include <worker/worker.h>
#include <worker/worker_pool.h>

std::uint64_t randomInt(int min, int max)
{
	static std::mt19937 gen{ std::random_device{}() };
	std::uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}

std::unique_ptr<DTPP::Task> createTask(std::uint64_t id) {

	return std::make_unique<DTPP::Task>(id, std::format("task {}", id),[id]() {

		std::cout << std::format("[task {}] Executing...\n", id);

		int waitTimeMs = randomInt(500, 1500);
		std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));

		std::cout << std::format("[task {}] Finished!\n", id);

		return DTPP::Task::Result{ DTPP::Task::Status::Completed, "Task 1 completed successfully", 0 };

	});

}

int main(int argc, char* argv[]) {

	const int nWorkers = 4;
	const int nTasks = 8;
	DTPP::ThreadSafeQueue queue{};

	DTPP::WorkerPool workerPool{ queue, nWorkers };
	workerPool.start();

	// Let's simulate tasks coming to the queue
	for (std::uint64_t i = 0; i < nTasks; ++i) {
		queue.push(createTask(i));
		int waitTimeMs = randomInt(100, 500);
		std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));
	}

	// Main loop
	while (true) {
		if (queue.empty()) {
			queue.stop();
			workerPool.stop();
			std::cout << std::format("[Main Loop] Work finished, bye!\n");
			break;
		}
		else {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	return 0;
}