#include <gtest/gtest.h>
#include <utility>
#include <functional>
#include <chrono>
#include <memory>

#include <test_utils.h>
#include <worker/worker.h>
#include <fakes/fake_queue.h>
#include <fakes/fake_task.h>


using namespace DTPP;

std::unique_ptr<FakeTask> createFakeTask(std::function<Task::Result()> work) {
	return std::make_unique<FakeTask>(0, work);
}
TEST(WorkerTest, StartStop_SubmitTasks_Completes) {
	FakeQueue queue{};

	bool fakeTaskExecuted = false;
	auto work = [&]() { 
		fakeTaskExecuted = true;
		return Task::Result{ true, "Completed", 0 };
	};

	bool taskStartedCalled = false;
	bool taskCompletedCalled = false;

	Worker<FakeQueue> worker(0, queue, 
		[&](Task::Id) { 
		taskStartedCalled = true;
	},
		[&](Task::Id, Task::Result) { 
		taskCompletedCalled = true; 
	});

	worker.start();

	// The queue is empty, so it shouldn't have executed anything yet
	EXPECT_EQ(false, fakeTaskExecuted);
	EXPECT_EQ(false, taskStartedCalled);
	EXPECT_EQ(false, taskCompletedCalled);
	queue.resetFake();

	queue.push(createFakeTask(work));

	{
		// Let's call until fakeTaskExecuted it's true (task executed)
		bool conditionSuccess = TestUtils::waitForConditionWithTimeout(
			std::chrono::milliseconds(3000),
			[&] { return fakeTaskExecuted; }
		);
		ASSERT_EQ(true, conditionSuccess);
	}

	{
		// Let's wait for the task to be completed
		bool conditionSuccess = TestUtils::waitForConditionWithTimeout(
			std::chrono::milliseconds(3000),
			[&] { return taskCompletedCalled; }
		);
		ASSERT_EQ(true, conditionSuccess);
	}

	// The task should actually have been completed
	EXPECT_EQ(true, taskStartedCalled);
	EXPECT_EQ(true, taskCompletedCalled);

	// Now let's requestStop it, no task should be executed
	fakeTaskExecuted = false;

	queue.resetFake();

	queue.stop();
	queue.push(createFakeTask(work));

	EXPECT_EQ(false, fakeTaskExecuted);

	worker.stopAndWait();
}
