#include <gtest/gtest.h>

#include <worker/worker_pool.h>
#include <fakes/fake_queue.h>

using namespace DTPP;

TEST(WorkerPool, StartStopTest) {
	FakeQueue queue{};

	int workerCount = 1;
	WorkerPool<FakeQueue> workerPool(queue, workerCount);

	workerPool.start();

	queue.stop(); // The workers are waiting at the condition var
	workerPool.stop();

	EXPECT_EQ(true, true);

	std::cout << std::format("WorkerPoolTest ended.\n");
}