#include <gtest/gtest.h>
#include <chrono>
#include <stdexcept>
#include <atomic>
#include <thread>
#include <future>

#include <common/task.h>
#include <scheduler/scheduler.h>
#include <test_utils.h>
#include <task_test.h>

using namespace DTPP;
using namespace std::chrono_literals;
using namespace TestUtils;

void waitForTaskToComplete(Scheduler& scheduler, Task::Id taskId) {
	auto timeout = std::chrono::seconds(3);
	TestUtils::waitForConditionWithTimeout(timeout, [&scheduler] {
		return scheduler.getTaskStatus(0) == Scheduler::Status::Completed;
	});
}

TEST(SchedulerTest, SubmitTask_CancelTasksAndWait_NotAllFinished) {
	const int N_TASKS = 8;
	const int N_WORKERS = 2;

	std::atomic<int> nTasksCompleted = 0;

	auto taskWork = [&nTasksCompleted](const bool& cancelRequested) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		nTasksCompleted++;
		return Task::Result{ true, "Completed", 0 };
	};

	Scheduler scheduler(N_WORKERS);

	// Let's submit the tasks, but wait for the first one
	auto task0Future = scheduler.submitTask(taskWork);
	for (int i = 1; i < N_TASKS; ++i) { scheduler.submitTask(taskWork); }

	// Check that there should be nTasks pending tasks
	for (Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(Scheduler::Status::Pending, scheduler.getTaskStatus(id));
	}

	scheduler.start();

	// Wait for the first one to finish, to make sure there is
	//  at least one that has completed
	auto taskInfo = task0Future.value().get();

	EXPECT_EQ(Scheduler::Status::Completed, taskInfo.status);

	// And now, let's cancel the rest
	scheduler.cancelTasksAndWait();

	// Check that not all the tasks have been finished
	EXPECT_TRUE(nTasksCompleted > 0);
	EXPECT_TRUE(nTasksCompleted < N_TASKS);

	// TODO check which tasks are completed and which tasks are cancelled.
	int nTasksStatusCompleted = 0;
	int nTasksStatusCancelled = 0;

	for (int i = 0; i < N_TASKS; i++) {
		auto status = scheduler.getTaskStatus(i);
		switch (status) {
		case Scheduler::Status::Completed: nTasksStatusCompleted++; break;
		case Scheduler::Status::Cancelled: nTasksStatusCancelled++; break;
		default: ASSERT_TRUE(false);
		}
	}

	EXPECT_EQ(N_TASKS, nTasksStatusCompleted + nTasksStatusCancelled);
	EXPECT_EQ(nTasksCompleted, nTasksCompleted);
}

TEST(SchedulerTest, SubmitTask_FinishTasksAndWait_AllFinished) {
	const int N_TASKS = 5;
	const int N_WORKERS = 2;

	std::atomic<int> nTasksCompleted = 0;

	auto task = [&nTasksCompleted](const bool& cancelRequested) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		nTasksCompleted++;
		return Task::Result{ true, "Completed", 0 };
	};

	Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Check that there should be nTasks pending tasks
	for (Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(Scheduler::Status::Pending, scheduler.getTaskStatus(id));
	}

	scheduler.start();

	// Let's wait until all the tasks have finished
	scheduler.finishTasksAndWait();

	// Check that there should be nTasks completed tasks
	for (Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(Scheduler::Status::Completed, scheduler.getTaskStatus(id));
	}
}


TEST(SchedulerTest, TrackTask_UnexistingTask_ThrowsException) {
	const int N_TASKS = 1;
	const int N_WORKERS = 4;

	auto task = [](const bool& cancelRequested) { return Task::Result{ true,	"Completed", 0 }; };

	Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Tracking a task with unexisting id throws an exception
	EXPECT_THROW(scheduler.getTaskStatus(555), std::out_of_range);
}

TEST(SchedulerTest, SubmitTask_TaskCompletes_CorrectTimeFields) {

	// Executing one task with one worker
	auto taskWork = [](const bool& cancelRequested) {
		// Task that takes 150 ms to finish
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
		return Task::Result{ true,	"Completed", 0 };;
	};

	Scheduler scheduler(1);

	scheduler.start();

	scheduler.submitTask(taskWork);

	EXPECT_TRUE(scheduler.getTaskStatus(0) != Scheduler::Status::Completed);

	waitForTaskToComplete(scheduler, 0);

	EXPECT_EQ(scheduler.getTaskStatus(0), Scheduler::Status::Completed);

	auto durationMs = scheduler.getTaskInfo(0).getDurationMs();

	EXPECT_TRUE(*durationMs > std::chrono::milliseconds(150));
}

TEST(SchedulerTest, SubmitTask_TaskDoesNotComplete_DoesNotHaveDuration) {

	std::promise<void> taskStartedPromise;
	std::promise<void> taskWorkPromise;

	// Executing one task with one worker
	auto taskWork = [&](const bool& cancelRequested) {
		taskStartedPromise.set_value();;

		// Let's simulate a task working for a long time
		taskWorkPromise.get_future().wait();

		return Task::Result{ true, "Completed", 0 };
	};

	Scheduler scheduler(1);

	scheduler.start();

	scheduler.submitTask(taskWork);

	// Let's wait for the task to actually start
	taskStartedPromise.get_future().wait();

	// This time we do not wait for the task to finish, so it should not be completed
	EXPECT_TRUE(scheduler.getTaskStatus(0) != Scheduler::Status::Completed);

	auto durationMs = scheduler.getTaskInfo(0).getDurationMs();

	EXPECT_FALSE(durationMs.has_value());

	// Shutdown
	taskWorkPromise.set_value();
	scheduler.cancelTasksAndWait();
}

TEST(SchedulerTest, SubmitTaskAndWait_WaitForTask_Completes) {
	

	auto taskWork = [](const bool& cancelRequested) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		return Task::Result{ true, "Completed", 0 };
	};

	Scheduler scheduler(1);

	scheduler.start();

	// Let's wait until all the tasks have finished
	auto taskFuture = scheduler.submitTask(taskWork);

	taskFuture.value().wait();

	auto taskInfo = taskFuture.value().get();
	auto taskStatus = scheduler.getTaskStatus(0);

	EXPECT_EQ(Scheduler::Status::Completed, taskStatus);
	EXPECT_EQ(0, taskInfo.id);
	EXPECT_EQ(true, taskInfo.result.value().success);
	EXPECT_EQ("Completed", taskInfo.result.value().message);
	EXPECT_EQ(0, taskInfo.result.value().data);
	EXPECT_TRUE(taskInfo.getDurationMs() > 0ms);
}

// TODO let's finish writing this one
TEST(SchedulerTest, CancelAllTasks_WaitForTask_Fullfillspromise) {


	// We create a cooperative task so that it can be cancelled
	//  half way and report it. Not as completed or failed, but
	//  as cancelled
	auto taskWork = [](const bool& cancelRequested) {
		for (int i = 0; i < 100; ++i) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			// TODO here we check if the tasks has been cancelled cooperatively
			// Return a resultState as completed, failed or cancelled
		}
		return Task::Result{ true, "Completed", 0 };
	};

	auto taskCompleted = std::make_unique<TaskTest>(0);
	auto taskCompletedTester = taskCompleted->tester();

	auto taskCancelled = std::make_unique<TaskTest>(0);
	auto taskCancelledTester = taskCancelled->tester();

	Scheduler scheduler(1);

	//scheduler.submitTask()

	scheduler.start();

	// We push the task, but do not unlock it's execution yet
	//// Let's wait until all the tasks have finished
	//auto taskFuture = scheduler.submitTask(taskWork);

	//taskFuture.value().wait();

	//auto taskInfo = taskFuture.value().get();
	//auto taskStatus = scheduler.getTaskStatus(0);

	//EXPECT_EQ(Scheduler::Status::Completed, taskStatus);
	//EXPECT_EQ(0, taskInfo.id);
	//EXPECT_EQ(true, taskInfo.result.value().success);
	//EXPECT_EQ("Completed", taskInfo.result.value().message);
	//EXPECT_EQ(0, taskInfo.result.value().data);
	//EXPECT_TRUE(taskInfo.getDurationMs() > 0ms);
}