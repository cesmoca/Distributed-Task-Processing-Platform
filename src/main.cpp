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
#include <future>
#include <optional>

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

std::function<DTPP::Task::Result(const bool&)> createWork() {

	return [](const bool& cancelRequested) {

		int waitTimeMs = randomInt(500, 1500) / 10;
		for (int i = 0; i < 10; ++i) {
			std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));
			if (cancelRequested) break;

		}
		return DTPP::Task::Result{ true, "Task completed successfully", 0 };
	};
}

int main(int argc, char* argv[]) {

	const int nWorkers = 4;
	const int nTasks = 10;

	std::vector<std::optional<std::future<DTPP::Scheduler::TaskInfo>>> tasksFutures(nTasks);
	DTPP::Scheduler scheduler{ nWorkers };

	scheduler.start();

	// Let's simulate tasks coming to the queue
	for (std::uint64_t i = 0; i < nTasks; ++i) {
		tasksFutures.at(i) = scheduler.submitTask(createWork());
		int waitTimeMs = randomInt(100, 500);
		std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));
	}

	// Let's wait for the first task to finish
	auto& task0Promise = tasksFutures.at(0);

	task0Promise.value().wait();
	auto task0Info = task0Promise.value().get();

	// Then cancel them all
	std::cout << std::format("[Main Loop] Cancelling tasks\n");
	scheduler.cancelTasksAndWait();

	// Show tasks results
	std::cout << task0Info.toString() << std::endl;

	for (DTPP::Task::Id id = 0; id < nTasks; ++id) {
		auto taskInfo = scheduler.getTaskInfo(id);
		std::cout << taskInfo.toString() << std::endl;
	}

	std::cout << std::format("[Main Loop] End reached\n");

	return 0;
}
