
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

		struct CancelRequest {
			bool request_cancel() { requested = true; }
			bool requested = false;
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

		template <typename Callable>
		requires std::same_as<std::invoke_result_t<Callable, const bool&>, Task::Result>
		Task(Id id, Callable&& work) : Task(id) {
			work_ = std::forward<Callable>(work);
		}

		Id id() const noexcept { return id_; }

		[[nodiscard]]
		Result execute(CancelRequest& cancelRequest) const;


	private:
		Id id_;
		std::function<Task::Result(bool&)> work_;

		Task(Id id) : id_(id) {}

	};

}

