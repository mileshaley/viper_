#include "literals.h"

namespace viper_::literals {

	detail::variable& operator""_(const char* string, size_t length) {
		return detail::variable_storage::global_context().map()[std::string(string, length)];
	}

	detail::variable& operator""_(uint64_t integer) {
		return detail::variable_storage::global_context().map()[std::to_string(integer)];
	}

	detail::variable& operator""_(long double floating) {
		return detail::variable_storage::global_context().map()[std::to_string(floating)];
	}

} // namespace viper_::literals
