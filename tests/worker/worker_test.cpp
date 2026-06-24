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
#include <test_utils.h>
#include <worker/worker.h>


using namespace DTPP;

// So the problem is that the thread is already waiting
//  for the next task, since it is empty. Then we order
//  the thread to finish all tasks and close. The expected
//  behaviour is that the thread gets out of that lock
//  and finishes. BUT it is the queue that can unlock it.
//  The worker does not have the capacity to unlock by itself
TEST(WorkerTest, FinishAllTasksAndStop_SubmitsTwoTasks_AllTaksComplete) {
	ThreadSafeQueue queue{};

	// Let's control when each tasks is finished
	std::promise<void> task0FinishPromise{}; 
	std::promise<void> task1FinishPromise{};

	std::atomic<bool> taskCompletedCalled = false;

	auto task0Work = [&]() { 
		task0FinishPromise.get_future().wait();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return Task::Result{ true, "Completed", 0 };
	};

	auto task1Work = [&]() {
		task1FinishPromise.get_future().wait();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return Task::Result{ true, "Completed", 0 };
	};

	Worker<ThreadSafeQueue> worker(0, queue,
		[&](Task::Id) { },
		[&](Task::Id, Task::Result) { 
		taskCompletedCalled = true;
	});

	worker.start();

	// The queue is empty, so it shouldn't have executed anything yet
	EXPECT_EQ(false, taskCompletedCalled);

	// Pushing two tasks
	queue.push(std::move(std::make_unique<Task>(DTPP::Task(0, task0Work))));
	queue.push(std::move(std::make_unique<Task>(DTPP::Task(1, task1Work))));

	task0FinishPromise.set_value(); // Unlock the first task's execution

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

	task1FinishPromise.set_value();
	EXPECT_EQ(false, taskCompletedCalled);

	worker.stop(DTPP::Worker<ThreadSafeQueue>::StopMode::FINISH_ALL_TASKS_AND_STOP);
	worker.waitUntilFinished();

	EXPECT_EQ(true, taskCompletedCalled);

	std::cout << "Main thread exiting\n";
}

TEST(WorkerTest, StopProcessingTasks_SubmitsTwoTasks_AllTaksComplete) {
	ThreadSafeQueue queue{};

	// Let's control when each tasks is finished
	std::promise<void> task0FinishPromise{};
	std::promise<void> task1FinishPromise{};

	std::atomic<bool> taskCompletedCalled = false;

	auto task0Work = [&]() {
		task0FinishPromise.get_future().wait();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return Task::Result{ true, "Completed", 0 };
	};

	auto task1Work = [&]() {
		task1FinishPromise.get_future().wait();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return Task::Result{ true, "Completed", 0 };
	};

	Worker<ThreadSafeQueue> worker(0, queue,
		[&](Task::Id) {},
		[&](Task::Id, Task::Result) {
		taskCompletedCalled = true;
	});

	worker.start();

	// The queue is empty, so it shouldn't have executed anything yet
	EXPECT_EQ(false, taskCompletedCalled);

	// Pushing two tasks
	queue.push(std::move(std::make_unique<Task>(DTPP::Task(0, task0Work))));
	queue.push(std::move(std::make_unique<Task>(DTPP::Task(1, task1Work))));

	task0FinishPromise.set_value(); // Unlock the first task's execution

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

	task1FinishPromise.set_value();
	EXPECT_EQ(false, taskCompletedCalled);

	worker.stop(DTPP::Worker<ThreadSafeQueue>::StopMode::STOP_PROCESSING_TASKS);
	worker.waitUntilFinished();

	EXPECT_EQ(true, taskCompletedCalled);

	std::cout << "Main thread exiting\n";
}
