#pragma once

#include <any> // variable data
#include <utility> // std::move
#include <type_traits>
#include "exception.h"
#include "hint.h"

namespace viper_::detail {

	class variable {
	public:
		variable();
		variable(variable const&) = delete;
		variable(variable&&) = delete;

		inline variable& operator=(variable const& rhs) {
			if (this == &rhs) { return *this; }
			check_assignment_type(rhs.m_data.type());
			m_data = rhs.m_data;
			create();
			return *this;
		}

		// Assignment
		// ----------
		
		template<typename T>
		/// TODO: Consider fixing pass by value (requires fixing constness type deduction issue)
		inline variable& operator=(T rhs) {
			check_assignment_type(typeid(std::decay_t<T>));
			m_data = rhs;
			create();
			return *this;
		}

		// To avoid issues with literal operator followed by .
		inline variable* operator->() {
			return this;
		}

		// Type Hinting
		// ------------

		// This one doesn't work yet due to operator precedence
		// It can be fixed if hint<T> is given an operator= that returns a proxy viper
		//template<typename T>
		//inline variable& operator=(hint<T>) {
		//	return this->hint<T>();
		//}

		template<typename T>
		inline constexpr variable& operator^(hint<T>) {
			return this->hint<T>();
		}

		template<typename T>
		inline constexpr variable& operator^(T) {
			return this->hint<T>();
		}

		template<typename T>
		inline variable& hint() {
			if (m_alive) {
				throw type_error("Cannot hint an initalized variable's type");
			}
			if (m_hint != nullptr) {
				throw type_error("Cannot hint a variable's type more than once");
			}
			m_hint = &typeid(std::decay_t<T>);
			return *this;
		}

		// Lifetime Utility
		// ----------------

		void create();
		void destroy();

	private:
		void check_assignment_type(std::type_info const& new_type);

		std::any m_data;
		std::type_info const* m_hint;
		bool m_alive;

	}; // class variable
} // namespace viper_::detail
