#include <gtest/gtest.h>
#include <chrono>
#include <stdexcept>

#include <scheduler/scheduler.h>
#include <test_utils.h>

TEST(SchedulerTest, SubmitTasks_CorrectTasks_Completes) {
	const int N_TASKS = 5;
	const int N_WORKERS = 2;
	
	int nTasksCompleted = 0;

	auto task = [&nTasksCompleted]() {
		nTasksCompleted++;
		return DTPP::Task::Result{
			true,
			"Completed",
			0
		};
	};

	DTPP::Scheduler scheduler(N_WORKERS);

	for (int i = 0; i < N_TASKS; ++i) { scheduler.submitTask(task); }

	// Check that there should be nTasks pending tasks
	for (DTPP::Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(DTPP::Task::Status::Pending, scheduler.trackTask(id));
	}

	scheduler.start();

	// Let's wait until all the tasks have finished, with a timeout
	TestUtils::waitForConditionWithTimeout(
		std::chrono::milliseconds(3000),
		[&] { return nTasksCompleted == N_TASKS; }
	);

	scheduler.stopAndWait();

	// Check that there should be nTasks completed tasks
	for (DTPP::Task::Id id = 0; id < N_TASKS; ++id) {
		EXPECT_EQ(DTPP::Task::Status::Completed, scheduler.trackTask(id));
	}
}

TEST(SchedulerTest, TrackTask_UnexistingTask_ThrowsException) {
	int nTasks = 1;
	int nTasksCompleted = 0;

	auto task = [&nTasksCompleted]() {
		nTasksCompleted++;
		return DTPP::Task::Result{
			true,
			"Completed",
			0
		};
	};

	DTPP::Scheduler scheduler(4);

	for (int i = 0; i < nTasks; ++i) { scheduler.submitTask(task); }

	// Tracking a task with unexisting id throws an exception
	EXPECT_THROW(scheduler.trackTask(555), std::out_of_range);
}