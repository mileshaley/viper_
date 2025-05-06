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

	"param3"_ = 2131;
	bar("param3"_, 78, 20);
	//bar("param3"_, "param3"_ = 1, "param2"_ = 2);

	print(f"param1 is {param1}, param2 is {param2}, param3 is {param3}");


	viper_::list my_list(3, "?", 4);

	//viper_::detail::type_record_storage::global_context().dump_type_names();
	
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


	/// TODO: This case breaks because the parameters are all evaluated in order first then the function is called.
	/// Currently the only way to fix this, as far as I can tell, is to make the value syntax state ever growing like with a vector.
	/// Although maybe it just needs to be 2 extra assignments instead of 1 because I can't see a valid case where you could write a variable's name at all 
	///		in a call to a function more than 2 times since you can only assign once within the parenthesis
	/// Another way to fix could be to make operator= of variable return a proxy with a pointer to the variable and some context about access stamps and value,
	///		this proxy could maybe be given a destructor that performs the assignment so that the state is preserved if the value of the expression is being used for the function call
	//"param3"_ = 2131;
	//bar("param3"_, "param3"_ = 1, "param2"_ = 2);

	return 0;
}
