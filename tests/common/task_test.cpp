#include <gtest/gtest.h>
#include <common/task.h>

using namespace DTPP;

TEST(TaskTest, Ctr) {

	Task task{ 0, "task", []() {
		return Task::Result{ Task::Status::Completed, "Task completed successfully", 42 };
	} };

	EXPECT_EQ(task.id(), 0);
	EXPECT_EQ(task.name(), "task");
}

TEST(TaskTest, Execute) {
	
	Task task{ 0, "task", []() {
		return Task::Result{ Task::Status::Completed, "Task completed successfully", 42 };
	} };

	Task::Result result = task.execute();

	EXPECT_EQ(result.status, Task::Status::Completed);
	EXPECT_EQ(result.message, "Task completed successfully");
	EXPECT_EQ(result.data, 42);

}