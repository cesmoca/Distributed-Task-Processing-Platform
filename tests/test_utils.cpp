#include <test_utils.h>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace TestUtils {
	bool waitForConditionWithTimeout(std::chrono::milliseconds timeoutMs, std::function<bool()> predicate) {
		
		auto deadline = std::chrono::steady_clock::now() + timeoutMs;

		while (!predicate())
		{
			if (std::chrono::steady_clock::now() >= deadline)
				return false;

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		return true;
	}
}
