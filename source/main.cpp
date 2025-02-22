#include "viper_.h"

int main() {
	auto const& a = typeid(std::decay_t<std::remove_reference_t<const char*>>);
	auto const& b = typeid(std::decay_t <decltype("hello world")>);
	const bool a_equal_b = a == b;
	(void)a_equal_b;

	auto h = hint(3);

	"hello"_ ^ hint(10) = 20;
	3.5_ ^ hint<double>() = 214.232;
	12_ _(int) = 1247891;
	"var2"_->hint<const char*>() = "world";
	print("hello! 12={12} and var2={var2} and 12={12} still");

	//int blug = 3;
	//float wung(felve) = 120.0f;
	//felvefelve = 5.01f * blug;

	return 0;
}
