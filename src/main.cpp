#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdio>

#include <common/thread_safe_queue.h>
#include <common/task.h>



int main(int argc, char* argv[]) {
	DTPP::ThreadSafeQueue queue{};

	DTPP::Task task1{ 0, "task1",[]() {

		std::printf("[task1] Executing...\n");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		std::printf("[task1] Finished!\n");

		return DTPP::Task::Result{ DTPP::Task::Status::Completed, "Task 1 completed successfully", 0 };

	} };
	

	return 0;
}