#include "viper_.h"




int main() {

	; ::viper_::detail::function bar = ::viper_::detail::function_builder{ "param1"_ = 2 } + [&]([[maybe_unused]] ::viper_::detail::function& __function__) {

	};
	
	; ::viper_::detail::function foo = ::viper_::detail::function_builder{ "param1"_ = 3, "param2"_ = "hello" } + [&]([[maybe_unused]] ::viper_::detail::function& __function__) {

	};
	

	"my_variable"_ = 100;
	// Also supports other literals:
	123_ = "Hello, World!";
	0.0_ = "my_variable"_;

	print(f"{123} This is a format print so my_variable = {my_variable}. Cool, right?");

	return 0;
}
