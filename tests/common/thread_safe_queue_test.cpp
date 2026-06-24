#include <gtest/gtest.h>
#include <memory>
#include <future>
#include <format>
#include <thread>

#include <common/thread_safe_queue.h>
#include <common/task.h>

using namespace DTPP;

TEST(ThreadSafeQueueTest, TryPopOrNull_PopTask_CorrectFields) {
	ThreadSafeQueue<Task> queue{};

	EXPECT_TRUE(queue.empty());

	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task != nullptr);
	EXPECT_EQ(task->id(), 0);
}

TEST(ThreadSafeQueueTest, TryPopOrNull_NotNullTask_CorrectFields) {
	ThreadSafeQueue<Task> queue{};

	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task != nullptr);
	EXPECT_EQ(task->id(), 0);
}

TEST(ThreadSafeQueueTest, TryPopOrNull_NullTask_ReturnsNullPtr) {
	ThreadSafeQueue<Task> queue{};

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task == nullptr);
}

TEST(ThreadSafeQueueTest, WaitAndPop_EmptyQueue_Completes) {
	ThreadSafeQueue<Task> queue{};
	
	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; } ));

	auto value = queue.waitAndPop();

	EXPECT_EQ(value->id(), 0);
}

TEST(ThreadSafeQueueTest, WaitAndPop_WaitsForTasks_Completes) {
	ThreadSafeQueue<Task> queue{};
	std::promise<void> promise; // It helps us with synchronization

	// Start with an empty queue, and we simulate
	//  a worker waiting for a task
	std::jthread producer([&] {
		promise.set_value();
		queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	});

	std::future<void> future = promise.get_future();
	future.get(); // Wait for producer to be ready

	auto value = queue.waitAndPop();
	EXPECT_EQ(value->id(), 0);
}

TEST(ThreadSafeQueueTest, Stop_PushTask_NotAdded) {
	ThreadSafeQueue<Task> queue{};
	
	auto work = [&]() {
		return Task::Result{ true, "Completed", 0 };
	};

	ASSERT_TRUE(queue.empty());

	queue.push(std::move(std::make_unique<Task>(0, work)));

	ASSERT_FALSE(queue.empty());

	// Let's pop the task
	auto task = queue.tryPopOrNull();
	ASSERT_TRUE(task != nullptr);

	// Should be empty again
	ASSERT_TRUE(queue.empty());

	queue.stop();

	queue.push(std::move(std::make_unique<Task>(0, work)));

	ASSERT_TRUE(queue.empty());
}
