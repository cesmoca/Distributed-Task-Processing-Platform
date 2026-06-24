#include <gtest/gtest.h>
#include <chrono>
#include <stdexcept>
#include <atomic>
#include <thread>
#include <future>

#include <scheduler/scheduler.h>
#include <test_utils.h>

TEST(SchedulerTest, SubmitTasks_CancelTasksAndWait_NotAllFinished) {
	const int N_TASKS = 5;
	const int N_WORKERS = 2;

	std::atomic<int> nTasksCompleted = 0;

	auto task = [&nTasksCompleted]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		nTasksCompleted++;
		return DTPP::Task::Result{ true, "Completed", 0 };
	};

	DTPP::Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Check that there should be nTasks pending tasks
	for (DTPP::Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(DTPP::Task::Status::Pending, scheduler.getTaskStatus(id));
	}

	scheduler.start();

	scheduler.cancelTasksAndWait();

	// Check that not all the tasks have been finished
	EXPECT_TRUE(nTasksCompleted < N_TASKS);
}

TEST(SchedulerTest, SubmitTasks_finishTasksAndWait_AllFinished) {
	const int N_TASKS = 5;
	const int N_WORKERS = 2;

	std::atomic<int> nTasksCompleted = 0;

	auto task = [&nTasksCompleted]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		nTasksCompleted++;
		return DTPP::Task::Result{ true, "Completed", 0 };
	};

	DTPP::Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Check that there should be nTasks pending tasks
	for (DTPP::Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(DTPP::Task::Status::Pending, scheduler.getTaskStatus(id));
	}

	scheduler.start();

	// Let's wait until all the tasks have finished
	scheduler.finishTasksAndWait();

	// Check that there should be nTasks completed tasks
	for (DTPP::Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(DTPP::Task::Status::Completed, scheduler.getTaskStatus(id));
	}
}


TEST(SchedulerTest, TrackTask_UnexistingTask_ThrowsException) {
	const int N_TASKS = 1;
	const int N_WORKERS = 4;

	auto task = []() { return DTPP::Task::Result{ true,	"Completed", 0 }; };

	DTPP::Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Tracking a task with unexisting id throws an exception
	EXPECT_THROW(scheduler.getTaskStatus(555), std::out_of_range);
}

TEST(SchedulerTest, SubmitTask_TaskCompletes_CorrectTimeFields) {

	// Executing one task with one worker
	auto task = []() {
		// Task that takes 150 ms to finish
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
		return DTPP::Task::Result{ true,	"Completed", 0 };;
	};

	DTPP::Scheduler scheduler(1);

	scheduler.start();

	scheduler.submitTask(task);

	EXPECT_TRUE(scheduler.getTaskStatus(0) != DTPP::Task::Status::Completed);

	// Let's wait for the task to finish
	auto timeout = std::chrono::seconds(3);
	TestUtils::waitForConditionWithTimeout(timeout, [&scheduler] {
		return scheduler.getTaskStatus(0) == DTPP::Task::Status::Completed;
	});

	EXPECT_EQ(scheduler.getTaskStatus(0), DTPP::Task::Status::Completed);

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

		return DTPP::Task::Result{ true, "Completed", 0 };
	};

	DTPP::Scheduler scheduler(1);

	scheduler.start();

	scheduler.submitTask(task);

	// Let's wait for the task to actually start
	taskStartedPromise.get_future().wait();

	// This time we do not wait for the task to finish, so it should not be completed
	EXPECT_TRUE(scheduler.getTaskStatus(0) != DTPP::Task::Status::Completed);

	auto durationMs = scheduler.getTaskInfo(0).getDurationMs();

	EXPECT_FALSE(durationMs.has_value());

	// Shutdown
	taskWorkPromise.set_value();
	scheduler.cancelTasksAndWait();
}
