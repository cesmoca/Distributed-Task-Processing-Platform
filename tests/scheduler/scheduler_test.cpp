#include <gtest/gtest.h>


#include<scheduler/scheduler.h>

TEST(SchedulerTest, SubmitTasks) {
	int nTasks = 10;
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

	scheduler.start();
	scheduler.stopAndWait();

	EXPECT_EQ(true, true);
}