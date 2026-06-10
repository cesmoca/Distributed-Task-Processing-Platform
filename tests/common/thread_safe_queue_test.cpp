#include <gtest/gtest.h>
#include <common/thread_safe_queue.h>

using namespace DTPP;

TEST(ThreadSafeQueueTest, WaitAndPop) {
	ThreadSafeQueue queue{};
	
	Task task{ 0, "task1", []() { return Task::Result{ DTPP::Task::Status::Failed, "Task 1 failed", -1 }; } };

	queue.push(std::move(task));

	Task value = queue.waitAndPop();

	EXPECT_EQ(value.id(), 0);
	EXPECT_EQ(value.name(), "task1");
}