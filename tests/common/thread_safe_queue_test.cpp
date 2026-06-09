#include <gtest/gtest.h>
#include <common/thread_safe_queue.h>

TEST(ThreadSafeQueueTest, WaitAndPop) {

	ThreadSafeQueue queue{};

	queue.push(42);
	int value = queue.waitAndPop();

	EXPECT_EQ(value, 42);

}