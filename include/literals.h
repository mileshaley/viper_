#pragma once

#include "variable_storage.h"

namespace viper_::literals {
	detail::variable& operator""_(const char* string, size_t length);
	detail::variable& operator""_(uint64_t integer);
	detail::variable& operator""_(long double floating);
} // namespace viper_::literals
