#pragma once
#include <chrono>
#include <functional>

namespace TestUtils {

	bool waitForConditionWithTimeout(std::chrono::milliseconds timeoutMs, std::function<bool()> predicate);

}