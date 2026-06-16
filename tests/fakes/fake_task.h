#pragma once

#include <cstdint>

class FakeTask {
public:

	template<typename Callable>
	FakeTask(std::uint64_t id, const std::string& name, Callable&& work) : FakeTask(id, name) {
		work_ = std::forward<Callable>(work);
	}

	DTPP::Task::Result execute() {
		executeCalled = true;
		work_();
		return taskResult;
	}

	std::uint64_t id() const noexcept { return id_; }
	const std::string& name() const noexcept { return name_; }

	DTPP::Task::Result taskResult = DTPP::Task::Result{ DTPP::Task::Status::Completed, "Completed", 0 };

	void resetFake() {
		executeCalled = false;
	}

	bool executeCalled = false;

private:
	std::uint64_t id_;
	std::string name_;
	std::function<DTPP::Task::Result()> work_;

	FakeTask(std::uint64_t id, const std::string& name) : id_(id), name_(name) {}

};