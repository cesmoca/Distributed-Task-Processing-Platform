#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <utility>

class FakeTask {
public:

	template<typename Callable>
	FakeTask(std::uint64_t id, Callable&& work) : FakeTask(id) {
		work_ = std::forward<Callable>(work);
	}

	DTPP::Task::Result execute() {
		executeCalled = true;
		work_();
		return taskResult;
	}

	DTPP::Task::Id id() const noexcept { return id_; }

	DTPP::Task::Result taskResult = DTPP::Task::Result{ true, "Completed", 0 };

	void resetFake() {
		executeCalled = false;
	}

	bool executeCalled = false;

private:
	DTPP::Task::Id id_;
	std::function<DTPP::Task::Result()> work_;

	FakeTask(DTPP::Task::Id id) : id_(id) {}

};