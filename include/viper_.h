#pragma once

/*~-------------------------------------------------------------------------~*\
 * All Shared Includes                                                       *
\*~-------------------------------------------------------------------------~*/

#include <stdexcept>
#include <stdint.h>
#include <utility>
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>
#include <unordered_map>
#include <any>
#include <functional>
#include <list>

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
} // namespace viper_

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

} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Values                                                                    *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class value {
	public:
		value(std::any&& data, bool is_data_mutable)
			: m_data(std::move(data))
			, m_mutable(is_data_mutable)
		{
		}

		bool is_mutable() const {
			return m_mutable;
		}

		inline std::type_info const& type() const {
			return m_data.type();
		}

		std::any const& data() const {
			return m_data;
		}

	private:
		std::any m_data;
		bool m_mutable;
	}; // class value

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Variables                                                                 *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class variable_stack;

	class variable {
	public: // Lifecycle

		variable()
			: m_data()
			, m_hint()
			, m_next_link(nullptr)
			, m_previous_link(nullptr)
			, m_owner(nullptr)
			, m_active(false)
			, m_unpack_count(0)
		{
		}

		variable(variable const& other)
			: m_data(other.m_data)
			, m_hint(other.m_hint)
			, m_next_link(nullptr)
			, m_previous_link(nullptr)
			, m_owner(nullptr)
			, m_active(other.m_active)
			, m_unpack_count(0)
		{
		}

		variable(variable&& other) noexcept
			: m_data(std::move(other.m_data))
			, m_hint(std::exchange(other.m_hint, nullptr))
			, m_next_link(std::exchange(other.m_next_link, nullptr))
			, m_previous_link(std::exchange(other.m_next_link, nullptr))
			, m_owner(nullptr)
			, m_active(std::exchange(other.m_active, false))
			, m_unpack_count(0)
		{
			if (m_previous_link) {
				m_previous_link->m_next_link = this;
			}
			if (m_next_link) {
				m_next_link->m_previous_link = this;
			}
		}

		~variable() {}

		inline void set_owner(variable_stack* owner) {
			m_owner = owner;
		}

		inline variable_stack* get_owner() {
			return m_owner;
		}

	public: // Assignment Operators

		// Rebind variable name to the value of another variable
		inline variable& operator=(variable const& rhs) {
			if (this == &rhs) { return *this; }
			//check_assignment_type(rhs.m_internal_value->type());
			m_data.assign(rhs.m_data.get());
			create();
			return *this;
		}

		/// TODO: rhs has to be passed by value otherwise T will be innacurate when const. Find a fix
		template<typename T>
		inline variable& operator=(T rhs) {
			// Explicitly instantiate reflection for T 
			(void)sizeof(instantiate_type<T>);
			//check_assignment_type(typeid(std::decay_t<T>));
			m_data.assign(std::make_shared<value>(rhs, true));
			create();
			return *this;
		}
			
	public: // Access Operator

		// To avoid issues with literal operator followed by .
		inline variable* operator->() {
			return this;
		}

	public: // Unpacking Operator

		inline variable* operator*() {
			if (++m_unpack_count > 2) {
				throw std::runtime_error("Variable unpacked more than twice.");
			}
			return this;
		}

	public: // Variable Linking

		// Link two variables together
		inline variable& operator,(variable& rhs) {
			m_next_link = &rhs;
			rhs.m_previous_link = this;
			return rhs;
		}

		inline bool is_linked() const {
			return m_previous_link || m_next_link;
		}

		inline variable* chain_begin() {
			if (!m_previous_link) { return this; }
			return m_previous_link->chain_begin();
		}

		inline variable* chain_end() {
			if (!m_next_link) { return this; }
			return m_next_link->chain_end();
		}

		inline variable* previous_link() {
			return m_previous_link;
		}

		inline variable* next_link() {
			return m_next_link;
		}
			
	//public: // Parameter Utilities
	//
	//	inline bool is_parameter() const {
	//		return m_parameter;
	//	}
	//	inline void set_as_parameter(bool new_state = true) {
	//		m_parameter = new_state;
	//	}

	public: // Type Hinting

		// This one doesn't work yet due to operator precedence
		// It can be fixed if hint<T> is given an operator= that returns a proxy variable
		//template<typename T>
		//inline variable& operator=(hint<T>) {
		//	return this->hint<T>();
		//}

		//template<typename T>
		//inline constexpr variable& operator^(hint<T>) {
		//	return this->hint<T>();
		//}

		//template<typename T>
		//inline constexpr variable& operator^(T) {
		//	return this->hint<T>();
		//}

		//template<typename T>
		//inline variable& hint() {
		//	// Explicitly instantiate T
		//	using instantiated = instantiate_type<T>;
		//	static instantiated g{};
		//	if (m_active) {
		//		throw type_error("Cannot hint an initalized variable's type");
		//	}
		//	if (m_hint != nullptr) {
		//		throw type_error("Cannot hint a variable's type more than once");
		//	}
		//	m_hint = &typeid(std::decay_t<T>);
		//	return *this;
		//}

			
	public: // Utility

		inline std::any const& data() const {
			return m_data.get()->data();
		}

		inline size_t type_hash_code() const {
			return m_data.get()->type().hash_code();
		}

		inline std::shared_ptr<value> steal_last_assignment() {
			return m_data.steal_last_assignment();
		}

		inline uint8_t steal_unpack_count() {
			return std::exchange(m_unpack_count, uint8_t(0));
		}
			 
	private: // Helpers

		inline void create() {
			m_active = true;
		}

		inline void destroy() {
			m_active = false;
			m_hint = nullptr;
			m_data = data_state();
		}

		//inline void check_assignment_type(std::type_info const& new_type) {
		//	if (m_active) {
		//		if (m_internal_value->type() != new_type) {
		//			throw type_error("Variable type was reassigned");
		//		}
		//	} else if (m_hint != nullptr && *m_hint != new_type) {
		//		throw type_error("Variable type does not match hint type");
		//	}
		//}

			
	private: // Member Variables

		// Encapsulate the state of value pointers
		class data_state {
		public:
			data_state()
				: m_value(nullptr)
				, m_last_assignment(nullptr)
			{}

			inline void assign(std::shared_ptr<value> const& new_value) {
				m_last_assignment = new_value;
			}

			inline void accept_last_assignment() const {
				if (m_last_assignment != nullptr) {
					m_value = m_last_assignment;
					m_last_assignment.reset();
				}
			}

			inline std::shared_ptr<value> const& get() const {
				accept_last_assignment();
				return m_value;
			}

			inline std::shared_ptr<value> steal_last_assignment() {
				return std::exchange(m_last_assignment, std::shared_ptr<value>());
			}

		private:
			mutable std::shared_ptr<value> m_value;
			mutable std::shared_ptr<value> m_last_assignment;
		};

		data_state m_data;

		std::type_info const* m_hint;
		variable* m_previous_link;
		variable* m_next_link;
		variable_stack* m_owner;

		bool m_active;
		uint8_t m_unpack_count;
	}; // class variable

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Variable Stack                                                            *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class variable_stack {
	public:
		variable_stack() 
			: m_data(1)
		{
			m_data.back().set_owner(this);
		}

		void pop() {
			if (m_data.size() <= 1llu) {
				m_data.pop_back();
			}
		}

		variable& push() {
			variable& new_variable = m_data.emplace_back();
			new_variable.set_owner(this);
			return new_variable;
		}

		variable& top() {
			return m_data.back();
		}

		variable const& top() const {
			return m_data.back();
		}

	private:
		std::list<variable> m_data;
	}; // class variable_storage

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Variable Storage                                                          *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class variable_storage {
	public:
		using map_type = std::unordered_map<std::string, variable_stack>;
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

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * String Representation of Data                                             *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	template<typename T, class = void>
	class has_to_string 
		: public std::false_type {};

	template<typename T>
	class has_to_string<T, std::void_t<decltype(std::to_string(std::declval<T>()))>> 
		: public std::true_type {};

	template<typename T>
	class string_representation {
	public:
		static inline std::string get(T const& data) {
			if constexpr (has_to_string<T>::value) {
				return std::to_string(data);
			} else {
				return {};
			}
		}
	}; // class string_representation<T>\

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
		static inline const type_instatiatior<T> global_instantiator{};
	}; // class type_instatiatior

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Literal Operators                                                         *
\*~-------------------------------------------------------------------------~*/

namespace viper_::literals {
	inline detail::variable& operator""_(const char* string, size_t length) {
		return detail::variable_storage::global_context().map()[std::string(string, length)].top();
	}

	inline detail::variable& operator""_(uint64_t integer) {
		return detail::variable_storage::global_context().map()[std::to_string(integer)].top();
	}

	inline detail::variable& operator""_(long double real) {
		return detail::variable_storage::global_context().map()[std::to_string(real)].top();
	}

	inline detail::variable& operator""_VIPER_UNDERSCORE(const char* string, size_t length) {
		return detail::variable_storage::global_context().map()[std::string(string, length)].top();
	}

	inline detail::variable& operator""_VIPER_UNDERSCORE(uint64_t integer) {
		return detail::variable_storage::global_context().map()[std::to_string(integer)].top();
	}

	inline detail::variable& operator""_VIPER_UNDERSCORE(long double real) {
		return detail::variable_storage::global_context().map()[std::to_string(real)].top();
	}

} // namespace viper_::literals

/*~-------------------------------------------------------------------------~*\
 * Functions                                                                 *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	class function {
		enum class parameter_type : int8_t {
			positional = 0,
			positional_with_default,
			positional_args,
			keyword,
			keyword_with_default,
			keyword_args
		};

	public:
		using callable_type = std::function<variable(function&)>;

		inline function(std::string&& name, std::vector<variable*> const& parameters, callable_type&& callable)
			: m_name(move(name))
			, m_parameters()
			, m_callable(move(callable))
		{
			enum parameter_phase : int {
				positional = 0,
				positional_with_default,
				keyword_args,
				finished
			} phase = positional;


			m_parameters.reserve(parameters.size());
			// Ensures we steal all last assignments and unpack counts from the parameters,
			// effectively resetting them to before the function was declared in case of an error downstream
			for (variable* parameter : parameters) {
				m_parameters.push_back({
					parameter->get_owner(),
					parameter->steal_last_assignment(),
					parameter_type::positional,
					parameter->steal_unpack_count()
				});
			}

			for (size_t i = 0; i < parameters.size(); ++i) {
				parameter& parameter = m_parameters[i];

				// Returns whether or not it changed the phase
				const auto unpack_change_phase = [&]() -> bool {
					if (parameter.unpack_count == 1) {
						if (phase >= keyword_args) {
							throw std::runtime_error("*arguments cannot appear more than once");
						} else if (parameter.default_value) {
							throw std::runtime_error("**keyword arguments cannot have a default value");
						}
						phase = keyword_args;
						parameter.type = parameter_type::positional_args;
						return true;
					} else if (parameter.unpack_count == 2) {
						if (parameter.default_value) {
							throw std::runtime_error("*arguments cannot have a default value");
						}
						// Args after **kwargs error handled below in finished case of phase switch
						phase = finished;
						parameter.type = parameter_type::keyword_args;
						return true;
					} else if (parameter.unpack_count >= 3) {
						throw std::runtime_error("Cannot put more than two '*' on an argument");
					}
					return false;
				};

				switch (phase) {
				case positional:
					if (!unpack_change_phase() && parameter.default_value) {
						phase = positional_with_default;
						parameter.type = parameter_type::positional_with_default;
					}
					break;
				case positional_with_default:
					if (!unpack_change_phase()) {
						if (parameter.default_value) {
							parameter.type = parameter_type::positional_with_default;
						} else {
							throw std::runtime_error("Argument without default value cannot follow arguments with default values");
						}
					}
					break;
				case keyword_args:
					if (!unpack_change_phase()) {
						if (parameter.default_value) {
							parameter.type = parameter_type::keyword_with_default;
						} else {
							parameter.type = parameter_type::keyword;
						}
					}
					break;
				case finished:
					throw std::runtime_error("Additional arguments not allowed after **keyword arguments");
				}
			}
		}

		inline ~function() = default;

		template<class... Args>
		inline variable operator()(Args const&... args) {
			for (auto const& [variable] : m_parameters) {
				variable->push();
			}

			const auto pop_variables = [&]() {
				for (auto const& [variable] : m_parameters) {
					variable->pop();
				}
			};

			try {
				preprocess_args<0, Args...>(args...);
			} catch (std::runtime_error& error) {
				pop_variables();
				throw error;
			}

			pop_variables();
		}

		std::string const& __name__ = m_name;

	private:
		template<size_t Index, class First, class... Rest>
		inline constexpr void preprocess_args(First const& first, Rest const&... rest) {
			
			std::decay_t<int> w;



			if constexpr (sizeof...(Rest) > 0) {
				preprocess_args<Index + 1, Rest...>(rest...);
			}
		}




	private:
		struct parameter {
			variable_stack* variable;
			std::shared_ptr<value> default_value;
			parameter_type type;
			uint8_t unpack_count;
		};

		std::string m_name;
		std::vector<parameter> m_parameters;
		callable_type m_callable;
	}; // class function



	// Helper class used in making a complete function object within the def macro
	class function_builder {
	public:
		inline function_builder(const char* name)
			: m_name(name)
			, m_parameters()
		{}

	private:
		template<class Callable, class = void>
		struct returns_void : std::false_type {};
		template<class Callable>
		struct returns_void<Callable, std::enable_if_t<std::is_void_v<std::invoke_result_t<Callable, function>>>> : std::true_type {};

	public:
		inline function_builder operator+(std::initializer_list<std::reference_wrapper<variable>> parameters) {
			m_parameters.clear(); // Just in case
			m_parameters.reserve(parameters.size());
			for (auto parameter : parameters) {
				m_parameters.emplace_back(&parameter.get());
			}
			return *this;
		}

		template<class Callable>
		inline constexpr function operator+(Callable const& callable) {
			if constexpr (returns_void<Callable>::value) {
				return { move(m_name), m_parameters,
					[&](function& function) -> variable {
						return variable(callable(function));
					}
				};
			} else {
				return { move(m_name), m_parameters,
					[&](function& function) -> variable {
						callable(function);
						return {};
					}
				};
			}
		}

	private:
		std::string m_name;
		std::vector<variable*> m_parameters;
	}; // class function_builder
} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Format Strings                                                            *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	inline void format_string_in_place(std::string& out) {
		int begin_format = -1;
		bool inside_format = false;
		for (int i = 0; i < out.size(); ++i) {
			if (out[i] == '{') {
				inside_format = true;
				begin_format = i;
			} else if (inside_format && out[i] == '}') {
				inside_format = false;
				const int variable_length = i - begin_format - 1;

				detail::variable_storage::map_type const& var_map
					= detail::variable_storage::global_context().map();
				detail::type_record_storage::map_type const& type_map
					= detail::type_record_storage::global_context().map();

				std::string data_string = "";

				auto var_it = var_map.find(out.substr(begin_format + 1llu, variable_length));
				if (var_it != var_map.end()) {
					auto type_it = type_map.find(var_it->second.top().type_hash_code());
					if (type_it != type_map.end()) {
						data_string = type_it->second->get_string_data(var_it->second.top().data());
					}
				}

				out.replace(begin_format, i - begin_format + 1, data_string);
				i += variable_length - 2; // -2 to account for the {}
			}
		}
	}
} // namespace viper_::detail

namespace viper_ {
	class format_string {
	public:
		inline format_string(const char* raw_string) 
			: m_raw(raw_string)
			, m_formatted(m_raw)
		{
			detail::format_string_in_place(m_formatted);
		}

		inline std::string const& formatted() const {
			return m_formatted;
		}

		inline std::string const& raw() const {
			return m_raw;
		}

		inline operator std::string const& () const {
			return m_formatted;
		}

	private:
		std::string m_raw;
		std::string m_formatted;
	}; // class format_string
} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Print Function                                                            *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {
	inline void print(std::string const& text) {
		std::cout << text << std::flush;
	}
} // namespace viper_

//namespace viper_::detail {
//
//	inline std::string evaluate_string_to_string(std::string_view str) {
//		size_t identifier_begin = 0;
//		bool in_identifier = false;
//	
//		detail::variable_storage::map_type const& var_map
//			= detail::variable_storage::global_context().map();
//		detail::type_record_storage::map_type const& type_map
//			= detail::type_record_storage::global_context().map();
//	
//		const auto get_variable = [&](size_t begin, size_t end) -> variable const* {
//			auto var_it = var_map.find(std::string(str.substr(begin, end - begin)));
//			if (var_it != var_map.end()) {
//				return &var_it->second;
//				//auto type_it = type_map.find(var_it->second.type_hash_code());
//				//if (type_it != type_map.end()) {
//				//	data_string = type_it->second->get_string_data(var_it->second.data());
//				//}
//			}
//		};
//	
//	
//		for (size_t i = 0; i < str.size(); ++i) {
//			
//			if (str[i] == ' ') {
//				if (in_identifier) {
//	
//				} else {
//					
//				}
//			}
//	
//		}
//	
//	
//	
//	
//	}
//} // namespace viper_::detail

//namespace viper_ {
//
//	inline void format_in_place(std::string& text) {
//		int begin_format = -1;
//		bool inside_format = false;
//		for (int i = 0; i < text.size(); ++i) {
//			if (inside_format) {
//				if (text[i] == '=') {
//					//const int expression_length = i - begin_format - 1;
//
//				}
//				else if (text[i] == '}') {
//
//					inside_format = false;
//					const int expression_length = i - begin_format - 1;
//
//					//detail::variable_storage::map_type const& var_map
//					//	= detail::variable_storage::global_context().map();
//					//detail::type_record_storage::map_type const& type_map
//					//	= detail::type_record_storage::global_context().map();
//
//					std::string data_string = "{?}";
//
//
//
//					text.replace(begin_format, i - begin_format + 1, data_string);
//					i += expression_length - 2; // -2 to account for {}
//				}
//			}
//			else {
//				if (text[i] == '{') {
//					inside_format = true;
//					begin_format = i;
//				}
//			}
//		}
//	}
//
//	inline void print(std::string text) {
//		format_in_place(text);
//		std::cout << text << std::endl;
//	}
//
//} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Underscore Proxy                                                          *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	class underscore_proxy {
	public: 
		inline underscore_proxy() {

		}
	}; // class underscore_proxy
} // namespace viper_::detail

// Intentionally located in the global namespace so _ macro can name this, the literal operator,
// or the macro depending on if parenthesis are present after the identifier
static inline viper_::detail::underscore_proxy _VIPER_UNDERSCORE;

// Has a leading underscore so that the name can also be used for a user-defined literal operator
#define _VIPER_UNDERSCORE(Type) VIPER_HINT(Type)

/*~-------------------------------------------------------------------------~*\
 * Constants                                                                 *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {
	static inline constexpr bool True = true;
	static inline constexpr bool False = false;
} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Macros                                                                    *
\*~-------------------------------------------------------------------------~*/

#define VIPER_HINT(Type) ^ ::viper_::hint<Type>()

#define VIPER_IN :
#define VIPER_ELIF else if
#define VIPER_EXCEPT catch
#define VIPER_FORMAT (::viper_::format_string)
#define VIPER_COLON

#define VIPER_INTERNAL_FILELINE (::std::string(__FILE__) + "?" + ::std::to_string(__LINE__))

#define VIPER_INTERNAL_DEF(...) ::std::initializer_list<std::reference_wrapper<::viper_::detail::variable>>{__VA_ARGS__} + [&]([[maybe_unused]] ::viper_::detail::function& __function__)

#define VIPER_DEF(Name) ; ::viper_::detail::function Name = ::viper_::detail::function_builder(#Name) + VIPER_INTERNAL_DEF

/*~-------------------------------------------------------------------------~*\
 * Preprocessor Control                                                      *
\*~-------------------------------------------------------------------------~*/

// Before including this file you may choose to do any combination of the following:
//     define VIPER_NO_NAMESPACE_POLLUTION to avoid global namespace pollution with short identifiers
//     define VIPER_NO_MACRO_POLLUTION to avoid global macro pollution (for macros like _ or f)

#if !defined(VIPER_NO_NAMESPACE_POLLUTION)
	using namespace viper_::literals;
	using viper_::hint;
	using viper_::type_error;
	using viper_::print;
	//using def = viper_::detail::function;

	using viper_::True;
	using viper_::False;
#endif // !defined(VIPER_NO_NAMESPACE_POLLUTION)

#if !defined(VIPER_NO_MACRO_POLLUTION)
	#define _ _VIPER_UNDERSCORE

	#define in VIPER_IN
	#define elif VIPER_ELIF
	#define except VIPER_EXCEPT
	#define f VIPER_FORMAT
	#define def VIPER_DEF

	#define col VIPER_COLON
#endif // !defined(VIPER_NO_MACRO_POLLUTION)


/*~-------------------------------------------------------------------------~*\
 * Upcoming Features                                                         *
\*~-------------------------------------------------------------------------~*/

// import
// classes
// modules and __name__
// exec() with dlls
// help()