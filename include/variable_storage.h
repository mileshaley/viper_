#pragma once

#include "variable.h" // variable_storage data
#include <string> // variable_storage map key type
#include <unordered_map> // variable_storage data
#include <stdint.h> // uint64_t

namespace viper_::detail {

	
	class variable_storage {
	public:
		using map_type = std::unordered_map<std::string, variable>;
		static variable_storage& global_context();

		variable_storage();
		map_type& map();

	private:
		map_type m_data;
	}; // class variable_storage

} //namespace viper_::detail

namespace viper_::literals {
	static detail::variable& operator""_(const char* string, size_t length) {
		return detail::variable_storage::global_context().map()[std::string(string, length)];
	}

	static detail::variable& operator""_(uint64_t integer) {
		return detail::variable_storage::global_context().map()[std::to_string(integer)];
	}

	static detail::variable& operator""_(long double floating) {
		return detail::variable_storage::global_context().map()[std::to_string(floating)];
	}
} // namespace viper_::literals