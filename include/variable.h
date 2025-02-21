#pragma once

#include <any> // variable data
#include <utility> // std::move
#include "exception.h"

namespace viper_::detail {
	class variable {
	public:
		variable();
		variable(variable const&) = delete;
		variable(variable&&) = delete;

		variable& operator=(variable const& rhs) {
			if (this == &rhs) { return *this; }
			check_assignment_type(rhs.m_data.type());
			m_data = rhs.m_data;
			create();
		}
		
		template<typename T>
		variable& operator=(T const& rhs) {
			if (this == &rhs) { return *this; }
			check_assignment_type(typeid(T));
			m_data = rhs;
			create();
		}

		/// move semantics might not be a great idea for viper_

		//variable& operator=(variable&& rhs) {
		//	if (this == &rhs) { return *this; }
		//	check_assignment_type(rhs.m_data.type());
		//	m_data = std::move(rhs.m_data);
		//	create();
		//}
		//
		//template<typename T>
		//variable& operator=(T&& rhs) {
		//	if (this == &rhs) { return *this; }
		//	check_assignment_type(typeid(T));
		//	m_data = std::move(rhs);
		//	create();
		//}

		template<typename T>
		void hint() {
			if (m_alive) {
				throw type_error("Cannot hint an initalized variable's type");
			}
			if (m_hint != nullptr) {
				throw type_error("Cannot hint a variable's type more than once");
			}

			m_hint = &typeid(T);
		}

		void create();
		void destroy();

	private:

		void check_assignment_type(std::type_info const& new_type);

		std::any m_data;
		std::type_info const* m_hint;
		bool m_alive;

	}; // class variable
} // namespace viper_::detail
