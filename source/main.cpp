#include "viper_.h"

int main() {
	
	def (foo)("param1"_ = "Hello", "param2"_ = "World!") {
		print(f"foo says: param1 is {param1}, param2 is {param2}");
	}

	def (bar)("param1"_, "param2"_ = true, "param3"_ = 2) {
		print(f"bar says: param1 is {param1}, param2 is {param2}, param3 is {param3}");

	};

	"param1"_ = "original1";
	"param2"_ = "original2";
	"param3"_ = "original3";

	//print(f"param1 is {param1}, param2 is {param2}, param3 is {param3}");
	//
	//bar(2);
	//
	//print(f"param1 is {param1}, param2 is {param2}, param3 is {param3}");
	//
	//bar(2, 3, 4);

	print(f"param1 is {param1}, param2 is {param2}, param3 is {param3}");

	bar("param3"_ = 3, "param1"_ = 1, "param2"_ = 2);

	print(f"param1 is {param1}, param2 is {param2}, param3 is {param3}");

	//foo();
	//
	//print(f"param1 is {param1}, param2 is {param2}");
	//
	//foo("passed1");
	//
	//print(f"param1 is {param1}, param2 is {param2}");


	"my_variable"_ = 100;
	// Also supports other literals:
	123_ = "Hello, World!";
	0.0_ = "my_variable"_;

	//print(f"{123} This is a format print so my_variable = {my_variable}. Cool, right?");

	/// Unpacking
	"first"_, *"all_middle"_, "last"_ = (1, 2, 3, 4, 5);

	return 0;
}
