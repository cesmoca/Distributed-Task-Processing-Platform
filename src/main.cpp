#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdio>
#include <random>
#include <utility>
#include <format>

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

DTPP::Task createTask(std::uint64_t id) {
	DTPP::Task task{ id, std::format("task {}", id),[id]() {

		std::cout << std::format("[task {}] Executing...\n", id);

		int waitTimeMs = randomInt(250, 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));

		std::cout << std::format("[task {}] Finished!\n", id);

		return DTPP::Task::Result{ DTPP::Task::Status::Completed, "Task 1 completed successfully", 0 };

	} };

	return task;
}

int main(int argc, char* argv[]) {

	const int nWorkers = 4;
	const int nTasks = 10;
	DTPP::ThreadSafeQueue queue{};

	DTPP::WorkerPool workerPool{ queue };

	// Let's simulate tasks coming to the queue
	for (std::uint64_t i = 0; i < nTasks; ++i) {
		queue.push(createTask(i));
	}

	workerPool.start(nWorkers);



	return 0;
}