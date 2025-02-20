#pragma once

#include <stdexcept>
#include <string>

namespace viper_ {
	class type_error : public std::runtime_error {
	public:
		explicit type_error(std::string const& message) 
			: runtime_error(message)
		{}

		explicit type_error(const char* message)
			: runtime_error(message) 
		{}
	private:
	};
} //namespace viper_
