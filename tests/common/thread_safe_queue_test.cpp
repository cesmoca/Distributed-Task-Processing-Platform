#include <gtest/gtest.h>
#include <common/thread_safe_queue.h>
#include <memory>
#include <future>
#include <format>
#include <thread>

using namespace DTPP;

TEST(ThreadSafeQueueTest, TryPopOrNull_PopTask_CorrectFields) {
	ThreadSafeQueue queue{};

	EXPECT_TRUE(queue.empty());

	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task != nullptr);
	EXPECT_EQ(task->id(), 0);
}

TEST(ThreadSafeQueueTest, TryPopOrNull_NotNullTask_CorrectFields) {
	ThreadSafeQueue queue{};

	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task != nullptr);
	EXPECT_EQ(task->id(), 0);
}

TEST(ThreadSafeQueueTest, TryPopOrNull_NullTask_ReturnsNullPtr) {
	ThreadSafeQueue queue{};

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task == nullptr);
}

TEST(ThreadSafeQueueTest, WaitAndPop_EmptyQueue_Completes) {
	ThreadSafeQueue queue{};
	
	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; } ));

	auto value = queue.waitAndPop();

	EXPECT_EQ(value->id(), 0);
}

TEST(ThreadSafeQueueTest, WaitAndPop_StoppingBeforeWait_Completes) {
	ThreadSafeQueue queue{};

	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	queue.stop();
	auto value = queue.waitAndPop();

	EXPECT_EQ(nullptr, value);
}

TEST(ThreadSafeQueueTest, WaitAndPop_WaitsForTasks_Completes) {
	ThreadSafeQueue queue{};
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



TEST(ThreadSafeQueueTest, Stop_RunsWaitAndPop_Completes) {
	ThreadSafeQueue queue{};
	std::promise<void> promise; // It helps us with synchronization

	// Start with an empty queue, and we simulate
	//  a worker waiting for a task
	std::jthread producer([&] {
		promise.set_value();
		queue.stop();

	});

	std::future<void> future = promise.get_future();
	future.get(); // Wait for producer to be ready to requestStop

	auto value = queue.waitAndPop();
	EXPECT_EQ(nullptr, value);
}
