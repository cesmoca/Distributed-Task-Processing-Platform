
#pragma once

#include <cstdint>
#include <string>
#include <cassert>
#include <functional>
#include <utility>

namespace DTPP {

	class Task
	{
	public:
		// TODO: Not using by now. We are implementing a callable based task,
		//  for simple implementation. In the future we will add types
		//  when using Networking integration, since a callable can
		//  not be serialized and sent over the network.
		enum class Type {
			Type1,
			Type2,
			Type3,
		};

		enum class Status {
			Pending,
			Running,
			Completed,
			Failed
		};

		// TODO: We are starting with just an enum, but in the future
		//  we can use a more complex structure, allowing for example
		//  to return all kinds of data. We can use variant for that
		struct Result {
			Status status;
			std::string message;
			int data; // Example of additional data

			std::string toString() const {
				switch (status) {
				case Status::Pending: return "Pending";
				case Status::Running: return "Running";
				case Status::Completed: return "Completed";
				case Status::Failed: return "Failed";
				default: assert("Not all Task::Result implemented in toString");

				}
			}
		};


		Task(std::uint64_t id, Type type, const std::string& name);

		template<typename Callable>
		Task(std::uint64_t id, const std::string& name, Callable&& work) : Task(id, name) {
			work_ = std::forward<Callable>(work);
		}

		Result execute() const;

		std::uint64_t id() const noexcept { return id_; }
		const std::string& name() const noexcept { return name_; }


	private:
		std::uint64_t id_;
		std::string name_;
		std::function<Result()> work_;

		Task(std::uint64_t id, std::string name) : id_(id), name_(std::move(name)) {}


	};


}

