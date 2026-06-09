#include <gtest/gtest.h>
#include <common/task.h>

using namespace DTPP;

TEST(TaskTest, Execute) {
	
	Task task1{ 0, "task1", []() {
		return Task::Result{ Task::Status::Completed, "Task completed successfully", 42 };
	} };

	Task::Result result = task1.execute();

	EXPECT_EQ(result.status, Task::Status::Completed);
	EXPECT_EQ(result.message, "Task completed successfully");
	EXPECT_EQ(result.data, 42);

}