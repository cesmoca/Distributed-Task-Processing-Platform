#include <gtest/gtest.h>
#include <common/task.h>

using namespace DTPP;

TEST(TaskTest, Ctr) {

	Task task{ 0, []() {
		return Task::Result{ true, "Task completed successfully", 42 };
	} };

	EXPECT_EQ(task.id(), 0);
}

TEST(TaskTest, Execute) {
	
	Task task{ 0, []() {
		return Task::Result{ true, "Task completed successfully", 42 };
	} };

	Task::Result result = task.execute();

	EXPECT_EQ(result.success, true);
	EXPECT_EQ(result.message, "Task completed successfully");
	EXPECT_EQ(result.data, 42);

}