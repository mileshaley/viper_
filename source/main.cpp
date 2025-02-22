#include "viper_.h"

def my_fn() {
	std::cout << "hello, world!";
}

int main() {
	"my_variable"_ = 100;
	// Also supports other literals:
	123_ = "Hello, World!";
	0.0_ = "my_variable"_;
	print("{123} This is a format print so my_variable = {my_variable}. Cool, right?");

	my_fn();
	return 0;
}
