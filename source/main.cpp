#include "viper_.h"

int main() {
	auto const& a = typeid(std::decay_t<std::remove_reference_t<const char*>>);
	auto const& b = typeid(std::decay_t <decltype("hello world")>);
	const bool a_equal_b = a == b;
	(void)a_equal_b;
	12_->hint<const char*>() = "4";

	auto h = hint(3);

	"hello"_ ^ hint(10) = 20;
	3.5_ ^ hint<double>() = 214.232;

	return 0;
}
