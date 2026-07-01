#include <gtest/gtest.h>
#include <utility>
#include <functional>
#include <chrono>
#include <memory>
#include <future>
#include <atomic>
#include <iostream>
#include <thread>

#include <common/task.h>
#include <worker/worker.h>
#include <test_utils.h>
#include <task_test.h>

using namespace DTPP;
using namespace TestUtils;

// So the problem is that the thread is already waiting
//  for the next task, since it is empty. Then we order
//  the thread to finish all tasks and close. The expected
//  behaviour is that the thread gets out of that lock
//  and finishes. BUT it is the queue that can unlock it.
//  The worker does not have the capacity to unlock by itself
TEST(WorkerTest, FinishAllTasksAndStop_SubmitsTwoTasks_AllTaksComplete) {
	ThreadSafeQueue<TaskTest> queue{};

	std::atomic<bool> taskCompletedCalled = false;

	auto task0 = std::make_unique<TaskTest>(0);
	auto task0Tester = task0->tester();

	auto task1 = std::make_unique<TaskTest>(0);
	auto task1Tester = task1->tester();

	Worker<TaskTest> worker(0, queue,
		[&](Task::Id) { },
		[&](Task::Id, Task::Result) { taskCompletedCalled = true; },
		[&](Task::Id) {});

	worker.start();

	// The queue is empty, so it shouldn't have executed anything yet
	EXPECT_EQ(false, taskCompletedCalled);

	// Pushing two tasks
	queue.push(std::move(task0));
	queue.push(std::move(task1));

	task0Tester->continueExecutionPromise.set_value(); // Unlock the first task's execution

	{
		// Let's wait for the first task to be completed
		bool conditionSuccess = TestUtils::waitForConditionWithTimeout(
			std::chrono::milliseconds(3000),
			[&] { return taskCompletedCalled.load(); }
		);
		ASSERT_EQ(true, conditionSuccess);
	}

	// The task should actually have been completed
	EXPECT_EQ(true, taskCompletedCalled);

	taskCompletedCalled = false;

	EXPECT_EQ(false, taskCompletedCalled);

	// Let's stop giving tasks, but the second one should finish
	queue.stop(); 

	task1Tester->continueExecutionPromise.set_value();

	EXPECT_EQ(false, taskCompletedCalled);

	worker.stop(Worker<TaskTest>::StopMode::FINISH_ALL_TASKS_AND_STOP);
	worker.waitUntilFinished();

	EXPECT_EQ(true, taskCompletedCalled);

	std::cout << "Main thread exiting\n";
}

TEST(WorkerTest, CancelTasksAndStop_SubmitsTwoTasks_AllTaksComplete) {
	ThreadSafeQueue<TaskTest> queue{};

	std::atomic<bool> taskCompletedCalled = false;

	auto task0 = std::make_unique<TaskTest>(0);
	auto task0Tester = task0->tester();

	auto task1 = std::make_unique<TaskTest>(0);
	auto task1Tester = task1->tester();

	Worker<TaskTest> worker(0, queue,
		[&](Task::Id) {},
		[&](Task::Id, Task::Result) { taskCompletedCalled = true; },
		[&](Task::Id) {});

	worker.start();

	// The queue is empty, so it shouldn't have executed anything yet
	EXPECT_EQ(false, taskCompletedCalled);

	// Pushing two tasks
	queue.push(std::move(task0));
	queue.push(std::move(task1));

	task0Tester->continueExecutionPromise.set_value(); // Unlock the first task's execution

	{
		// Let's wait for the first task to be completed
		bool conditionSuccess = TestUtils::waitForConditionWithTimeout(
			std::chrono::milliseconds(3000),
			[&] { return taskCompletedCalled.load(); }
		);
		ASSERT_EQ(true, conditionSuccess);
	}

	// The task should actually have been completed
	EXPECT_EQ(true, taskCompletedCalled);

	taskCompletedCalled = false;

	EXPECT_EQ(false, taskCompletedCalled);

	// Let's stop giving tasks, but the second one should finish
	queue.stop();

	task1Tester->continueExecutionPromise.set_value();

	EXPECT_EQ(false, taskCompletedCalled);

	worker.stop(Worker<TaskTest>::StopMode::CANCEL_TASKS_AND_STOP);
	worker.waitUntilFinished();

	EXPECT_EQ(true, taskCompletedCalled);

	std::cout << "Main thread exiting\n";
}
