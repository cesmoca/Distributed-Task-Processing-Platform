#pragma once

#include <functional>
#include <future>
#include <chrono>
#include <mutex>

#include <common/task.h>


namespace TestUtils {

	class TaskTest {

	public:

		struct Tester {
		public:
			std::chrono::milliseconds execTime = std::chrono::milliseconds(0);
			DTPP::Task::Result result{ true, "Completed", 0 };
			std::promise<void> continueExecutionPromise;
		};

		TaskTest(DTPP::Task::Id id): id_(id){}

		DTPP::Task::Id id() const noexcept { return id_; }

		[[nodiscard]]
		DTPP::Task::Result execute() const {
			work_();
			return tester_->result;
		}

		std::shared_ptr<Tester> tester() {
			return tester_;
		}

	private:
		DTPP::Task::Id id_;
		std::shared_ptr<Tester> tester_ = std::make_shared<Tester>();


		std::function<DTPP::Task::Result()> defaultWork_ = [this] {
			tester_->continueExecutionPromise.get_future().wait();
			std::this_thread::sleep_for(tester_->execTime);
			return tester_->result;
		};

		std::function<DTPP::Task::Result()> work_ = defaultWork_;

	};
}