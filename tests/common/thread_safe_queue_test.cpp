#include <gtest/gtest.h>
#include <common/thread_safe_queue.h>
#include <memory>
#include <future>
#include <format>

using namespace DTPP;

TEST(ThreadSafeQueueTest, Push) {
	ThreadSafeQueue queue{};

	EXPECT_TRUE(queue.empty());

	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task != nullptr);
	EXPECT_EQ(task->info().id, 0);

	std::cout << std::format("ThreadSafeQueueTest ended\n");
}

TEST(ThreadSafeQueueTest, WaitAndPopEmptyQueue) {
	ThreadSafeQueue queue{};
	
	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; } ));

	auto value = queue.waitAndPop();

	EXPECT_EQ(value->info().id, 0);
}

TEST(ThreadSafeQueueTest, WaitAndPopStoppingBeforeWait) {
	ThreadSafeQueue queue{};

	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	queue.stop();
	auto value = queue.waitAndPop();

	EXPECT_EQ(nullptr, value);
}

TEST(ThreadSafeQueueTest, WaitAndPopWaitsForTasks) {
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
	EXPECT_EQ(value->info().id, 0);
}

TEST(ThreadSafeQueueTest, TryPopOrNull_NotNull) {
	ThreadSafeQueue queue{};

	queue.push(std::make_unique<Task>(0, []() { return Task::Result{ false, "Task 1 failed", -1 }; }));

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task != nullptr);
	EXPECT_EQ(task->info().id, 0);
}

TEST(ThreadSafeQueueTest, TryPopOrNull_Null) {
	ThreadSafeQueue queue{};

	auto task = queue.tryPopOrNull();

	EXPECT_TRUE(task == nullptr);
}

TEST(ThreadSafeQueueTest, Stop) {
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