#include "viper_.h"




int main() {
	
	def (foo)("param1"_ = 3, "param2"_ = "hello") {

	}

	def (bar)() {

	};
	
	"my_variable"_ = 100;
	// Also supports other literals:
	123_ = "Hello, World!";
	0.0_ = "my_variable"_;

	print(f"{123} This is a format print so my_variable = {my_variable}. Cool, right?");

	return 0;
}
