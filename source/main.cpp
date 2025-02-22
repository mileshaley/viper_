#include "viper_.h"

#if 0 // Sample

def add_res("a"_, "b"_)
_	"sum"_ = "a"_ + "b"_
_	"statement"_ = f"sum {a}+{b}={sum} is: "
_	if_ "sum"_ > 0
_	_	print("statement"_ + "positive")
_	elif_ "sum"_ < 0
_	_	print("statement"_ + "negative")
_	elif_ "sum"_ == 0
_	_	print("statement"_ + "equal to 0")
_	return_ ""

#endif

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
