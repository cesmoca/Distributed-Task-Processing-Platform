#include <iostream>
#include <string>
#include <vector>

class Wrapper {
public:

	Wrapper(std::initializer_list<int> list) : vec_(list) { std::cout << "> Ctr Initializer list" << std::endl; }
	Wrapper(size_t size) : vec_(size) { std::cout << "> Ctr Vector size" << std::endl; }
	Wrapper() : vec_() { std::cout << "> Default constructor" << std::endl; }

	// Copy constructor
	Wrapper(const Wrapper& other) : vec_(other.vec_) { std::cout << "> Copy constructor" << std::endl; }

	// Copy assignment
	Wrapper& operator=(const Wrapper& other) {
		std::cout << "> Copy assignment" << std::endl;
		if (this != &other) {
			vec_ = other.vec_;
		}
		return *this;
	}

	// Move constructor
	Wrapper(Wrapper&& other) noexcept : vec_(std::move(other.vec_)) { 
		std::cout << "> Move constructor" << std::endl; 
	}

	// Move assignment
	Wrapper& operator=(Wrapper&& other) noexcept {
		std::cout << "> Move assignment" << std::endl;
		if (this != &other) {
			vec_ = std::move(other.vec_);
		}
		return *this;
	}

	// Destructor
	~Wrapper() { std::cout << "> Destructor" << std::endl; }

	std::string toString() const {
		std::string line;
		for (const auto& val : vec_) {
			line += std::to_string(val) + " ";
		}
		return line;
	}

private:

	std::vector<int> vec_;

};

void fun12(const Wrapper& wrapper) {
	//return std::move(Wrapper{ 1, 2, 3, 4, 5 });
	//Wrapper wrapper{ 1, 2, 3, 4, 5 };
	//return std::move(Wrapper{ 1, 2, 3, 4, 5 });
}



int main(int argc, char* argv[]) {
	Wrapper local{ 1, 2, 3, 4, 5 };

	std::cout << ">> First case" << std::endl;
	fun12(Wrapper{ 1, 2, 3, 4, 5 });
	//fun12(local);
	//std::cout << "Wrapper at the end" << local.toString() << std::endl;
	
	
	//fun12(std::move(wrapper));
	//fun12(std::move(Wrapper{ 1, 2, 3, 4, 5 }));

	//std::cout << ">> Second case" << std::endl;
	

	//std::cout << ">> Third case" << std::endl;

	return 0;
}

//Wrapper fun22() {
//	Wrapper wrapper{ 1, 2, 3, 4, 5 };
//	std::cout << "Inside before move: " << wrapper.toString() << std::endl;
//	return wrapper;
//}
//
//void fun2() {
//
//	Wrapper wrapper(1);
//	std::cout << "Outside before move: " << wrapper.toString() << std::endl;
//	wrapper = std::move(fun22());
//	std::cout << "Outside after move: " << wrapper.toString() << std::endl;
//}

static_assert(std::is_nothrow_move_constructible_v<Wrapper>);