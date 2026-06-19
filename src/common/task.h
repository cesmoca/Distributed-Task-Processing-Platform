
#pragma once

#include <cstdint>
#include <string>
#include <cassert>
#include <functional>
#include <utility>
#include <stdexcept>
#include <format>
#include <optional>
#include <chrono>

namespace DTPP {

	class Task
	{
	public:

		using Id = std::uint64_t;
		using Timestamp = std::optional<std::chrono::time_point<std::chrono::steady_clock>>;
		using DurationMs = std::optional<std::chrono::milliseconds>;

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

		struct Result {
			bool success;
			std::string message;
			int data; // Example of additional data
			std::optional<std::string> errorMsg;

			std::string toString() const {
				return std::format("Result {}: {}, data: {}", success ? "Succeeded" : "Failed", message, data);
			}
		};

		template<typename Callable>
		Task(Id id, Callable&& work) : Task(id) {
			work_ = std::forward<Callable>(work);
		}

		Id id() const noexcept { return id_; }
		Result execute() const;


	private:
		Id id_;
		std::function<Result()> work_;

		Task(Id id) : id_(id) {}

	};

}

