#include <worker/worker.h>
#include <common/task.h>
#include <format>
#include <iostream>

using namespace DTPP;

void Worker::start() {
	std::cout << std::format("[Worker {}] Starting worker thread...\n", id_);

	// Worker thread logic goes here
	// TODO Just running run for now
	run();
}

void Worker::stop() {
	std::cout << std::format("[Worker {}] Stopping worker thread...\n", id_);

	// Logic to stop the worker thread goes here
	// TODO

	//stopToken_.request_stop();
	stopToken_stopRequested_ = true;

}

void Worker::run() {
	std::cout << std::format("[Worker {}] Running worker thread...\n", id_);

	//while (!stopToken_.stop_requested()) {
	while (!stopToken_stopRequested_) {

		Task task = queue_.waitAndPop(); // Wait for a task to be available in the queue

		task.execute();
	}

	std::cout << std::format("[Worker {}] Worker finished\n", id_);

}