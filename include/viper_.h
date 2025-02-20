#pragma once

#include <unordered_map>
#include <any>

namespace viper {

	class variable {
	public:
		variable();

	private:
		std::any m_data;

	}; // class variable


} // namespace viper


