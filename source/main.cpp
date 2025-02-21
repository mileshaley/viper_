#include "viper_.h"

int main() {
	auto const& a = typeid(std::decay_t<std::remove_reference_t<const char*>>);
	auto const& b = typeid(std::decay_t <decltype("hello world")>);
	bool a_equal_b = a == b;
	12_->hint<const char*>() = "je;";

	return 0;
}
