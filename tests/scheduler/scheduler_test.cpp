#include <gtest/gtest.h>
#include <chrono>
#include <stdexcept>
#include <atomic>
#include <thread>
#include <future>

#include <common/task.h>
#include <scheduler/scheduler.h>
#include <test_utils.h>

using namespace DTPP;
using namespace std::chrono_literals;

void waitForTaskToComplete(Scheduler& scheduler, Task::Id taskId) {
	auto timeout = std::chrono::seconds(3);
	TestUtils::waitForConditionWithTimeout(timeout, [&scheduler] {
		return scheduler.getTaskStatus(0) == Task::Status::Completed;
	});
}
TEST(SchedulerTest, SubmitTask_CancelTasksAndWait_NotAllFinished) {
	const int N_TASKS = 5;
	const int N_WORKERS = 2;

	std::atomic<int> nTasksCompleted = 0;

	auto task = [&nTasksCompleted]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		nTasksCompleted++;
		return Task::Result{ true, "Completed", 0 };
	};

	Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Check that there should be nTasks pending tasks
	for (Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(Task::Status::Pending, scheduler.getTaskStatus(id));
	}

	scheduler.start();

	scheduler.cancelTasksAndWait();

	// Check that not all the tasks have been finished
	EXPECT_TRUE(nTasksCompleted < N_TASKS);
}

TEST(SchedulerTest, SubmitTask_finishTasksAndWait_AllFinished) {
	const int N_TASKS = 5;
	const int N_WORKERS = 2;

	std::atomic<int> nTasksCompleted = 0;

	auto task = [&nTasksCompleted]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		nTasksCompleted++;
		return Task::Result{ true, "Completed", 0 };
	};

	Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Check that there should be nTasks pending tasks
	for (Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(Task::Status::Pending, scheduler.getTaskStatus(id));
	}

	scheduler.start();

	// Let's wait until all the tasks have finished
	scheduler.finishTasksAndWait();

	// Check that there should be nTasks completed tasks
	for (Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(Task::Status::Completed, scheduler.getTaskStatus(id));
	}
}


TEST(SchedulerTest, TrackTask_UnexistingTask_ThrowsException) {
	const int N_TASKS = 1;
	const int N_WORKERS = 4;

	auto task = []() { return Task::Result{ true,	"Completed", 0 }; };

	Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Tracking a task with unexisting id throws an exception
	EXPECT_THROW(scheduler.getTaskStatus(555), std::out_of_range);
}

TEST(SchedulerTest, SubmitTask_TaskCompletes_CorrectTimeFields) {

	// Executing one task with one worker
	auto task = []() {
		// Task that takes 150 ms to finish
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
		return Task::Result{ true,	"Completed", 0 };;
	};

	Scheduler scheduler(1);

	scheduler.start();

	scheduler.submitTask(task);

	EXPECT_TRUE(scheduler.getTaskStatus(0) != Task::Status::Completed);

	waitForTaskToComplete(scheduler, 0);

	EXPECT_EQ(scheduler.getTaskStatus(0), Task::Status::Completed);

	auto durationMs = scheduler.getTaskInfo(0).getDurationMs();

	EXPECT_TRUE(*durationMs > std::chrono::milliseconds(150));
}

TEST(SchedulerTest, SubmitTask_TaskDoesNotComplete_DoesNotHaveDuration) {

	std::promise<void> taskStartedPromise;
	std::promise<void> taskWorkPromise;

	// Executing one task with one worker
	auto task = [&]() {
		taskStartedPromise.set_value();;

		// Let's simulate a task working for a long time
		taskWorkPromise.get_future().wait();

		return Task::Result{ true, "Completed", 0 };
	};

	Scheduler scheduler(1);

	scheduler.start();

	scheduler.submitTask(task);

	// Let's wait for the task to actually start
	taskStartedPromise.get_future().wait();

	// This time we do not wait for the task to finish, so it should not be completed
	EXPECT_TRUE(scheduler.getTaskStatus(0) != Task::Status::Completed);

	auto durationMs = scheduler.getTaskInfo(0).getDurationMs();

	EXPECT_FALSE(durationMs.has_value());

	// Shutdown
	taskWorkPromise.set_value();
	scheduler.cancelTasksAndWait();
}

TEST(SchedulerTest, SubmitTaskAndWait_WaitForTask_Completes) {
	

	auto taskWork = []() {
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

	EXPECT_EQ(Task::Status::Completed, taskStatus);
	EXPECT_EQ(0, taskInfo.id);
	EXPECT_EQ(true, taskInfo.result.value().success);
	EXPECT_EQ("Completed", taskInfo.result.value().message);
	EXPECT_EQ(0, taskInfo.result.value().data);
	EXPECT_TRUE(taskInfo.getDurationMs() > 0ms);
}
