#include <gtest/gtest.h>
#include <common/task.h>

using namespace DTPP;

TEST(TaskTest, Ctr_CorrectFields) {

	Task task{ 0, [](const bool& cancelRequested) {
		return Task::Result{ true, "Task completed successfully", 42 };
	} };

	EXPECT_EQ(task.id(), 0);
}

TEST(TaskTest, Execute_CorrectTask_Completes) {
	
	Task task{ 0, [](const bool& cancelRequested) {
		return Task::Result{ true, "Task completed successfully", 42 };
	} };

	Task::CancelRequest cancelRequest;
	Task::Result result = task.execute(cancelRequest);

	EXPECT_EQ(result.success, true);
	EXPECT_EQ(result.message, "Task completed successfully");
	EXPECT_EQ(result.data, 42);

}