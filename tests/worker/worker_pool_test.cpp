#include <gtest/gtest.h>
#include <format>
#include <iostream>

#include <common/task.h>
#include <common/thread_safe_queue.h>
#include <worker/worker_pool.h>

using namespace DTPP;

TEST(WorkerPool, StartStop_Completes) {
	ThreadSafeQueue<Task> queue{};

	int workerCount = 1;
	WorkerPool<ThreadSafeQueue<Task>> workerPool(
		queue, 
		workerCount,
		[](Task::Id) {},
		[](Task::Id, Task::Result) {});

	workerPool.start();

	queue.stop(); // The workers are waiting at the condition var
	workerPool.stopAndWait(DTPP::Worker<ThreadSafeQueue<Task>>::StopMode::FINISH_ALL_TASKS_AND_STOP);

	std::cout << std::format("WorkerPoolTest ended.\n");
}
