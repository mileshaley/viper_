#pragma once

/*~-------------------------------------------------------------------------~*\
 * All Shared Includes                                                       *
\*~-------------------------------------------------------------------------~*/

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <any> // variable data
#include <utility> // move
#include <type_traits> // decay_t
#include <iostream> // cout
#include <memory> // unique_ptr

/*~-------------------------------------------------------------------------~*\
 * Forward Declarations                                                      *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	template<typename T>
	class instantiate_type;

} // namespace viper_::detail

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
	}; // class type_error
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
	}; // class hint

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
			// Explicitly instantiate T
			using instantiated = instantiate_type<T>;
			static instantiated g2{};
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
			// Explicitly instantiate T
			using instantiated = instantiate_type<T>;
			static instantiated g{};
			if (m_alive) {
				throw type_error("Cannot hint an initalized variable's type");
			}
			if (m_hint != nullptr) {
				throw type_error("Cannot hint a variable's type more than once");
			}
			m_hint = &typeid(std::decay_t<T>);
			return *this;
		}

			// Utility //
	public: // ------- //

		std::any const& data() const {
			return m_data;
		}

		inline size_t type_hash_code() const {
			return m_data.type().hash_code();
		}

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
 * String Representation of Data                                             *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	template<typename T>
	class string_representation {
	public:
		static inline std::string get(T const& data) {
			return std::to_string(data);
		}
	}; // class string_representation<T>

	template<>
	class string_representation<const char*> {
	public:
		static inline std::string get(const char* const& data) {
			return std::string(data);
		}
	}; // class string_representation<cosnt char*>

	template<>
	class string_representation<std::string> {
	public:
		static inline std::string get(std::string const& data) {
			return data;
		}
	}; // class string_representation<std::string>

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Type Records                                                              *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	class type_record {
	public:
		type_record() = default;
		virtual ~type_record() = default;
		virtual std::string get_string_data(std::any const& data) const = 0;
	}; // class type_record

	template<typename T>
	class typed_type_record : public type_record {
	public:
		using type = T;
		typed_type_record() = default;
		virtual ~typed_type_record() override = default;

		virtual std::string get_string_data(std::any const& data) const override {
			return string_representation<T>::get(std::any_cast<T const&>(data));
		}
	}; // class type_record

} //namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Type Record Storage                                                       *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class type_record_storage {
	public:
		using map_type = std::unordered_map<size_t, std::unique_ptr<type_record>>;
		static inline type_record_storage& global_context() {
			static type_record_storage storage{};
			return storage;
		}

		type_record_storage() = default;

		inline map_type& map() {
			return m_data;
		}

		template<typename T>
		inline void register_type(size_t key) {
			m_data.try_emplace(key, static_cast<type_record*>(new typed_type_record<T>()));
		}

	private:
		map_type m_data;
	}; // class type_record_storage

} //namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Type Instantiation                                                        *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	template<typename T>
	class type_instatiatior {
	public:
		using type = T;

		inline type_instatiatior() {
			type_record_storage::global_context().register_type<T>(
				typeid(std::decay_t<T>).hash_code());
		}
	}; // class type_instatiatior

	template<typename T>
	class instantiate_type {
	public:
		static inline type_instatiatior<T> global_instantiator{};
	}; // class type_instatiatior

} // namespace viper_::detail

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
 * Format Print Function                                                     *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {
	inline void print(std::string const& text) {
		std::string out(text);
		int begin_format = -1;
		bool inside_format = false;
		for (int i = 0; i < out.size(); ++i) {
			if (out[i] == '{') {
				inside_format = true;
				begin_format = i;
			}
			else if (inside_format && out[i] == '}') {
				inside_format = false;
				const int variable_length = i - begin_format - 1;

				detail::variable_storage::map_type const& var_map 
					= detail::variable_storage::global_context().map();
				detail::type_record_storage::map_type const& type_map 
					= detail::type_record_storage::global_context().map();
				
				std::string data_string = "";

				auto var_it = var_map.find(out.substr(begin_format + 1llu, variable_length));
				if (var_it != var_map.end()) {
					auto type_it = type_map.find(var_it->second.type_hash_code());
					if (type_it != type_map.end()) {
						data_string = type_it->second->get_string_data(var_it->second.data());
					}
				}

				out.replace(begin_format, i - begin_format + 1, data_string);
				i += variable_length - 2; // -2 to account for {}
			}
		}
		std::cout << out << std::endl;
	}
} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Underscore Proxy                                                          *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Macros                                                                    *
\*~-------------------------------------------------------------------------~*/

#define VIPER_HINT(Type) ^ ::viper_::hint<Type>()
#define VIPER_UNDERSCORE 1223

/*~-------------------------------------------------------------------------~*\
 * Preprocessor Control                                                      *
\*~-------------------------------------------------------------------------~*/

// Before including this file you may choose to do any combination of the following:
//     define VIPER_NO_NAMESPACE_POLLUTION to avoid global namespace pollution
//     define VIPER_NO_MACRO_POLLUTION to avoid global macro pollution (for macros like _)

#if !defined(VIPER_NO_NAMESPACE_POLLUTION)
	using namespace viper_::literals;
	using viper_::hint;
	using viper_::type_error;
	using viper_::print;
#endif // !defined(VIPER_NO_NAMESPACE_POLLUTION)

#if !defined(VIPER_NO_MACRO_POLLUTION)
	#define _ VIPER_UNDERSCORE
#endif // !defined(VIPER_NO_MACRO_POLLUTION)
