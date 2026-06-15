#include <gtest/gtest.h>
#include <worker/worker.h>
#include <fakes/fake_queue.h>
#include <fakes/fake_task.h>
#include <utility>

using namespace DTPP;

TEST(WorkerTest, StartStop) {
	FakeQueue queue{};

	bool fakeTaskExecuted = false;
	auto fakeTask = std::make_unique<FakeTask>(0, "FakeTask", [&]() { 
		fakeTaskExecuted = true;
		return Task::Result{ Task::Status::Completed, "Completed", 0 };
	});


	Worker<FakeQueue> worker{0, queue};
	worker.start();

	// Let's give time to the worker to start it's thread
	auto future = queue.promise.get_future();
	future.get();

	// The queue is empty, so it shouldn't have executed anything yet
	EXPECT_EQ(false, fakeTaskExecuted);

	queue.push(std::move(fakeTask));
	
	// Now there is an element, so it should execute the task
	EXPECT_EQ(true, fakeTaskExecuted);

	// Now let's stop it, no task should be executed
	fakeTaskExecuted = false;
	worker.stop();
	queue.push(std::move(fakeTask));

	// Give it time to execute the task if it was alive
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	EXPECT_EQ(false, fakeTaskExecuted);

}