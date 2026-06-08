
#include <cstdint>
#include <string>
#include <variant>

class Task
{
	public:
		
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

		using Result = std::variant<int, std::string, double>;

		Task(const std::uint64_t id, const Type type, const std::string name) {

		}
	



};