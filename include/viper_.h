#pragma once

/*~-------------------------------------------------------------------------~*\
 * All Shared Includes                                                       *
\*~-------------------------------------------------------------------------~*/

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <stdint.h>

#include <any> // variable data
#include <utility>
#include <type_traits>

/*~-------------------------------------------------------------------------~*\
 * Errors & Exceptions                                                       *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {
	class type_error : public std::runtime_error {
	public:
		explicit type_error(std::string const& message)
			: runtime_error(message) {
		}

		explicit type_error(const char* message)
			: runtime_error(message) {
		}
	private:
	};
} //namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Type Hints                                                                *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {

	template<typename T>
	class hint {
	public:
		using type = T;
		inline constexpr hint() {}
		inline constexpr hint(T) {}
	};

	// Deduction guide
	template<typename T>
	hint(T) -> hint<T>;

} //namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Variables                                                                 *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class variable {
			// Lifecycle
	public: // ---------
		variable(variable const&) = delete;
		variable(variable&&) = delete;

		variable()
			: m_data()
			, m_alive(false)
			, m_hint() {
		}

			// Assignment Operators
	public: // --------------------

		inline variable& operator=(variable const& rhs) {
			if (this == &rhs) { return *this; }
			check_assignment_type(rhs.m_data.type());
			m_data = rhs.m_data;
			create();
			return *this;
		}



		template<typename T>
		/// TODO: Consider fixing pass by value (requires fixing constness type deduction issue)
		inline variable& operator=(T rhs) {
			check_assignment_type(typeid(std::decay_t<T>));
			m_data = rhs;
			create();
			return *this;
		}


			// Access Operators
	public: // ----------------

		// To avoid issues with literal operator followed by .
		inline variable* operator->() {
			return this;
		}

			// Type Hinting
	public: // ------------

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

			// Lifetime Utility //
	public: // ---------------- //

		inline void create() {
			m_alive = true;
		}

		inline void destroy() {
			m_alive = false;
			m_hint = nullptr;
			m_data.reset();
		}
			 // Helpers //
	private: // ------- //

		inline void check_assignment_type(std::type_info const& new_type) {
			if (m_alive) {
				if (m_data.type() != new_type) {
					throw type_error("Variable type was reassigned");
				}
			} else if (m_hint != nullptr && *m_hint != new_type) {
				throw type_error("Variable type does not match hint type");
			}
		}

			 // Member Variables //
	private: // ---------------- //

		std::any m_data;
		std::type_info const* m_hint;
		bool m_alive;

	}; // class variable

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Variable Storage                                                          *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class variable_storage {
	public:
		using map_type = std::unordered_map<std::string, variable>;
		static inline variable_storage& global_context() {
			static variable_storage storage{};
			return storage;
		}

		inline variable_storage()
			: m_data()
		{}

		inline map_type& map() {
			return m_data;
		}

	private:
		map_type m_data;
	}; // class variable_storage

} //namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Literal Operators                                                         *
\*~-------------------------------------------------------------------------~*/

namespace viper_::literals {

	inline detail::variable& operator""_(const char* string, size_t length) {
		return detail::variable_storage::global_context().map()[std::string(string, length)];
	}

	inline detail::variable& operator""_(uint64_t integer) {
		return detail::variable_storage::global_context().map()[std::to_string(integer)];
	}

	inline detail::variable& operator""_(long double floating) {
		return detail::variable_storage::global_context().map()[std::to_string(floating)];
	}

} // namespace viper_::literals

/*~-------------------------------------------------------------------------~*\
 * Preprocessor Control                                                      *
\*~-------------------------------------------------------------------------~*/

// Define VIPER_NO_NAMESPACE_POLLUTION before this file is
// included to avoid viper polluting the global namespace
#if !defined(VIPER_NO_NAMESPACE_POLLUTION)
using namespace viper_::literals;
using viper_::hint;
using viper_::type_error;
#endif // !defined(VIPER_NO_NAMESPACE_POLLUTION)
