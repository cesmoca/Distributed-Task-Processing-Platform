#include <gtest/gtest.h>
#include <worker/worker.h>
#include <fakes/fake_queue.h>
#include <fakes/fake_task.h>
#include <utility>
#include <functional>

using namespace DTPP;

std::unique_ptr<FakeTask> createFakeTask(std::function<Task::Result()> work) {
	return std::make_unique<FakeTask>(0, work);
}
TEST(WorkerTest, StartStop) {
	FakeQueue queue{};
	std::promise<void> taskExecutedPromise{};

	bool fakeTaskExecuted = false;
	auto work = [&]() { 
		fakeTaskExecuted = true;
		taskExecutedPromise.set_value();
		return Task::Result{ true, "Completed", 0 };
	};

	Worker<FakeQueue> worker{0, queue};
	worker.start();

	// Let's give time to the worker to start it's thread
	queue.promise.get_future().get(); // Wait for the worker to wait on waitAndPop

	// The queue is empty, so it shouldn't have executed anything yet
	EXPECT_EQ(false, fakeTaskExecuted);
	queue.resetFake();

	queue.push(createFakeTask(work));
	taskExecutedPromise.get_future().get(); // Wait for the worker to get the task
	
	// Now there is an element, so it should execute the task
	EXPECT_EQ(true, fakeTaskExecuted);

	// Now let's requestStop it, no task should be executed
	fakeTaskExecuted = false;
	taskExecutedPromise = std::promise<void>{};
	queue.resetFake();

	queue.stop();
	queue.push(createFakeTask(work));

	EXPECT_EQ(false, fakeTaskExecuted);

	worker.requestStop(); // This is a blocking API, joins the thread
	worker.join();

	std::cout << std::format("Worker test ended.\n");
}