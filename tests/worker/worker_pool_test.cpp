#include <gtest/gtest.h>
#include <format>
#include <iostream>

#include <worker/worker_pool.h>
#include <fakes/fake_queue.h>

using namespace DTPP;

TEST(WorkerPool, StartStop_Completes) {
	FakeQueue queue{};

	int workerCount = 1;
	WorkerPool<FakeQueue> workerPool(
		queue, 
		workerCount,
		[](Task::Id) {},
		[](Task::Id, Task::Result) {});

	workerPool.start();

	queue.stop(); // The workers are waiting at the condition var
	workerPool.stopAndWait();

	std::cout << std::format("WorkerPoolTest ended.\n");
}
