#include <worker/worker_pool.h>
#include <cstdint>

void DTPP::WorkerPool::start(int workerCount){

	// Create and start the worker counts
	for (int i = 0; i < workerCount; ++i) {
		Worker worker{ static_cast<std::uint64_t>(i), queue_ };
		worker.start();
		workers_.emplace_back(std::move(worker));
	}
}

void DTPP::WorkerPool::stop(){

	// Stop all the workers
	for (auto& worker : workers_) {
		worker.stop();
	}

}