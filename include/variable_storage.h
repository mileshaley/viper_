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
