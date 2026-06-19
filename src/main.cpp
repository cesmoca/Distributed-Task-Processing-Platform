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
#include <functional>
#include <cstdint>

#include <common/thread_safe_queue.h>
#include <common/task.h>
#include <worker/worker.h>
#include <worker/worker_pool.h>
#include <scheduler/scheduler.h>

std::uint64_t randomInt(int min, int max)
{
	static std::mt19937 gen{ std::random_device{}() };
	std::uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}

std::function<DTPP::Task::Result()> createTask() {

	return []() {

		int waitTimeMs = randomInt(500, 1500);
		std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));

		return DTPP::Task::Result{ true, "Task completed successfully", 0 };
	};
}

int main(int argc, char* argv[]) {

	const int nWorkers = 4;
	const int nTasks = 10;
	
	DTPP::Scheduler scheduler{nWorkers};
	scheduler.start();

	// Let's simulate tasks coming to the queue
	for (std::uint64_t i = 0; i < nTasks; ++i) {
		scheduler.submitTask(createTask());
		int waitTimeMs = randomInt(100, 500);
		std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));
	}

	scheduler.stopAndWait();

	// Show tasks results
	for (DTPP::Task::Id id = 0; id < nTasks; ++id) {
		auto taskInfo = scheduler.getTaskInfo(id);
		std::cout << taskInfo.toString() << std::endl;
	}

	std::cout << std::format("[Main Loop] End reached\n");

	return 0;
}
