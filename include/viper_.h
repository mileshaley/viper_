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
#include <format>

/*~-------------------------------------------------------------------------~*\
 * Macros                                                                    *
\*~-------------------------------------------------------------------------~*/

#define VIPER_HINT(Type) ^ ::viper_::hint<Type>()

#define VIPER_IN :
#define VIPER_ELIF else if
#define VIPER_EXCEPT catch
#define VIPER_FORMAT (::viper_::format_string)
#define VIPER_COLON

#define VIPER_INTERNAL_DEF(...) ::std::initializer_list<std::reference_wrapper<::viper_::detail::variable>>{__VA_ARGS__} + [&]([[maybe_unused]] ::viper_::detail::function& __function__)
#define VIPER_DEF(Name) ::viper_::detail::function Name = ::viper_::detail::function_builder(#Name) + VIPER_INTERNAL_DEF

#define VIPER_INTERNAL_NO_CAPTURE_DEF(...) ::std::initializer_list<std::reference_wrapper<::viper_::detail::variable>>{__VA_ARGS__} + []([[maybe_unused]] ::viper_::detail::function& __function__)
#define VIPER_NO_CAPTURE_DEF(Name) ::viper_::detail::function Name = ::viper_::detail::function_builder(#Name) + VIPER_INTERNAL_NO_CAPTURE_DEF

/*~-------------------------------------------------------------------------~*\
 * Preprocessor Control                                                      *
\*~-------------------------------------------------------------------------~*/

// Before including this file you may choose to do any combination of the following:
//     define VIPER_NO_NAMESPACE_POLLUTION to avoid global namespace pollution with short identifiers
//     define VIPER_NO_MACRO_POLLUTION to avoid global macro pollution (for macros like _ or f)

#if not defined(VIPER_NO_MACRO_POLLUTION)
#define _ _VIPER_UNDERSCORE

#define in VIPER_IN
#define elif VIPER_ELIF
#define except VIPER_EXCEPT
#define f VIPER_FORMAT
#define def_ VIPER_DEF
#define def VIPER_NO_CAPTURE_DEF
#define col VIPER_COLON
/// TODO: Add 'global' keyword that works for variables like how f keyword works
#endif // not defined(VIPER_NO_MACRO_POLLUTION)

/*~-------------------------------------------------------------------------~*\
 * Internal Macros                                                           *
\*~-------------------------------------------------------------------------~*/

#define VIPER_INTERNAL_FILELINE (::std::string(__FILE__) + "?" + ::std::to_string(__LINE__))
#define VIPER_INTERNAL_INSTANTIATE_TYPE(Type) (void)sizeof(::viper_::detail::instantiate_type<Type>)

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
	namespace detail {
		class exception : public std::runtime_error {
		public:
			exception(std::string const& message)
				: std::runtime_error(message)
			{}
			exception(const char* message)
				: std::runtime_error(message)
			{}

		}; // class exception
	} // namespace detail

	class type_error : public detail::exception {
	public:
		explicit type_error(std::string const& message)
			: detail::exception(message) 
		{}
		explicit type_error(const char* message)
			: detail::exception(message) 
		{}

	}; // class type_error

	class syntax_error : public detail::exception {
	public:
		explicit syntax_error(std::string const& message)
			: detail::exception(message)
		{}
		explicit syntax_error(const char* message)
			: detail::exception(message)
		{}

	}; // class type_error

	class index_error : public detail::exception {
	public:
		explicit index_error(std::string const& message)
			: detail::exception(message) 
		{}
		explicit index_error(const char* message)
			: detail::exception(message) 
		{}
	};

	class value_error : public detail::exception {
	public:
		explicit value_error(std::string const& message)
			: detail::exception(message) {
		}
		explicit value_error(const char* message)
			: detail::exception(message) {
		}
	};
} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Program Termination Handling                                              *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class custom_termination_handler_instantiator {
	public:

		[[noreturn]] static inline void handler() {
			if (std::exception_ptr exception = std::current_exception()) {
				try {
					std::rethrow_exception(exception);
				} catch (type_error const& error) {
					std::cerr << "TypeError: " << error.what() << std::endl;
					std::exit(3);
				} catch (syntax_error const& error) {
					std::cerr << "SyntaxError: " << error.what() << std::endl;
					std::exit(3);
				} catch (index_error const& error) {
					std::cerr << "IndexError: " << error.what() << std::endl;
					std::exit(3);
				} catch (value_error const& error) {
					std::cerr << "ValueError: " << error.what() << std::endl;
					std::exit(3);
				}
			}
			std::abort();
		}

		custom_termination_handler_instantiator() {
			std::set_terminate(handler);
		}
	};

	static inline const custom_termination_handler_instantiator custom_termination_handler_instantiator_instance{};

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Object Class                                                              *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {
	class object {

	};
} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * String Representation of Data                                             *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	static inline const std::string unknown_string_representation = "<?>";

	template<typename T, class = void>
	class has_to_string 
		: public std::false_type {};

	template<typename T>
	class has_to_string<T, std::void_t<decltype(std::to_string(std::declval<T>()))>>
		: public std::true_type {};

	template<typename T, class = void>
	class string_convertible
		: public std::false_type {
	};
	template<typename T>
	class string_convertible<T, std::void_t<decltype(std::string(std::declval<T>()))>>
		: public std::true_type {};



	template<typename T>
	class string_representation {
	public:
		static inline std::string get(T const& data) {
			if constexpr (string_convertible<T>::value) {
				return data;
			} else if constexpr (has_to_string<T>::value) {
				return std::to_string(data);
			} else {
				return unknown_string_representation;
			}
		}
	}; // class string_representation<T>

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Type Records                                                              *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	template<typename T, class = void>
	struct has_equals : std::false_type {};
	template<typename T>
	struct has_equals<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> : std::true_type {};

	class type_record {
	public:
		type_record() = default;
		virtual ~type_record() = default;
		virtual std::string get_string_data(std::any const& data) const = 0;
		virtual const char* get_type_name() const = 0;
		virtual bool equal(std::any const& a, std::any const& b) const = 0;
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

		virtual const char* get_type_name() const override {
			return typeid(T).name();
		}

		virtual bool equal(std::any const& a, std::any const& b) const override {
			if constexpr (has_equals<T>::value) {
				if (a.type() != b.type() or not a.has_value() or not b.has_value()) {
					return false;
				}
				return std::any_cast<std::decay_t<T>>(a) == std::any_cast<std::decay_t<T>>(b);
			} else {
				return false;
			}
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

		type_record const* find(size_t key) const {
			const auto it = m_data.find(key);
			if (it == m_data.end()) { return nullptr; }
			return it->second.get();
		}

		template<typename T>
		inline void register_type(size_t key) {
			m_data.try_emplace(key, static_cast<type_record*>(new typed_type_record<T>()));
		}

		void dump_type_names() const {
			for (auto const& record : m_data) {
				std::cout << record.second->get_type_name() << std::endl;
			}
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
		static inline const type_instatiatior<T> instance{};
	}; // class type_instatiatior

} // namespace viper_::detail

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
 * Value Data & Metadata                                                     *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class value_data {
	public:
		value_data(std::any const& data, bool data_is_mutable)
			: m_data(data)
			, m_mutable(data_is_mutable) 
		{
		}

		value_data(std::any&& data, bool data_is_mutable)
			: m_data(std::move(data))
			, m_mutable(data_is_mutable)
		{
		}

		bool is_mutable() const {
			return m_mutable;
		}

		inline std::type_info const& type() const {
			return m_data.type();
		}

		std::any const& get_data_storage() const {
			return m_data;
		}

		std::any& get_data_storage() {
			return m_data;
		}

	private:
		std::any m_data;
		bool m_mutable;
	}; // class value_data

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Value Class                                                               *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class value {
	public: // Lifecycle

		inline value()
			: m_data(nullptr)
		{
		}

		/// TODO: Remove nullptr and T constructors and make it more explicit. Why? What if the user wanted to store a nullptr in a value?
		/// Perhaps we should make a make_value function to match shared_ptr semantics
		
		inline value(std::nullptr_t)
			: m_data(nullptr)
		{
		}

		template<typename T>
		inline value(T data)
			: m_data(std::make_shared<value_data>(std::move(data), true))
		{
			VIPER_INTERNAL_INSTANTIATE_TYPE(std::decay_t<T>);
		}

		inline value(value const& other)
			: m_data(other.m_data)
		{
		}

		inline value(value&& other) noexcept
			: m_data(std::move(other.m_data))
		{
		}

		inline void reset() {
			m_data.reset();
		}

		template<typename T>
		inline void assign(std::shared_ptr<value_data> const& new_data) {
			m_data = new_data;
		}

		inline void assign(value const& new_value) {
			m_data = new_value.m_data;
		}

		value& operator=(value const& rhs) {
			m_data = rhs.m_data;
		}


	public: // Value Access

		inline value_data& data() {
			return *m_data;
		}

		inline value_data const& data() const {
			return *m_data;
		}

	public: // Comparison
		inline bool is_truthy() const {
			/// TODO: This needs to actually get implemented
			return m_data != nullptr;
		}

		inline bool is_same(value const& rhs) const {
			return m_data == rhs.m_data;
		}

		inline bool is_none() const {
			return m_data == nullptr;
		}

		inline bool is_some() const {
			return m_data != nullptr;
		}

		template<typename T>
		inline bool operator==(T const& rhs) const {
			VIPER_INTERNAL_INSTANTIATE_TYPE(std::decay_t<T>);
			if constexpr (has_equals<T>::value) {
				if (not m_data or not m_data->get_data_storage().has_value() or m_data->type() != typeid(std::decay_t<T>)) {
					return false;
				}
				return std::any_cast<std::decay_t<T>>(m_data->get_data_storage()) == static_cast<std::decay_t<const T>>(rhs);
			} else {
				return false;
			}
		}

		inline bool operator==(value const& rhs) const {
			if (not m_data or not rhs.m_data) { 
				if (not m_data and not rhs.m_data) {
					return true;
				}
				return false; 
			}

			auto const& type_records = type_record_storage::global_context();
			
			type_record const* my_type = type_records.find(m_data->type().hash_code());
			/// TODO: Maybe this should be an error
			if (my_type == nullptr) { return false; }
			return my_type->equal(m_data->get_data_storage(), rhs.m_data->get_data_storage());
		}

	private: // Data Member
		std::shared_ptr<value_data> m_data;
	}; // class value

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Value to String Conversion                                                *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	template<>
	class string_representation<value> {
	public:
		static inline std::string get(value const& data) {
			if (data.is_none()) {
				return "None";
			}
			type_record const* record = type_record_storage::global_context().find(data.data().get_data_storage().type().hash_code());
			if (record == nullptr) {
				return unknown_string_representation;
			}
			return record->get_string_data(data.data().get_data_storage());
		}
	}; // class string_representation<cosnt char*>
} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Variable Access Stamps                                                    *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail::stamp {
	using counter_t = uint32_t;
	static_assert(std::is_unsigned_v<counter_t>, "Stamp counters must be unsigned to allow for looping");
	// Works even when unsigned counter overflows as long as 
	// their difference isn't more than 2^(size in bits - 1)
	static inline constexpr bool is_newer(counter_t current, counter_t previous) {
		// View unsigned difference as signed, turning the high bit into the sign bit
		// This means: (current < previous) -> negative, (current > previous) -> positive
		return static_cast<std::make_signed_t<counter_t>>(current - previous) > 0;
	}
} // namespace viper_::detail::stamp

/*~-------------------------------------------------------------------------~*\
 * Variable Stack                                                            *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	class variable;
	// generic_variable_stack is a template to workaround the circular dependency between variable_stack and variable
	// It can only be used with type variable and should only ever be referred to as variable_stack
	template<typename Variable = variable>
	class generic_variable_stack {
		static_assert(std::is_same_v<Variable, variable>, "generic_variable_stack only should be used with variable");
	public: // Lifecycle
		inline generic_variable_stack(std::string const& name = "__unnamed__")
			// Invariant: there exists a minimum of 1 variable on a variable stack
			: m_data(1, this)
			, m_name(name)
			, m_access_counter(0)
		{
		}

		inline void pop() {
			if (m_data.size() > 1llu) {
				m_data.pop_back();
			}
		}

		inline Variable& push() {
			Variable& new_variable = m_data.emplace_back(this);
			return new_variable;
		}

		inline Variable& top() {
			return m_data.back();
		}

		inline Variable const& top() const {
			return m_data.back();
		}

		inline std::string const& get_name() const {
			return m_name;
		}

	public: // Counter

		inline void increment_access_count() {
			++m_access_counter;
		}

		inline stamp::counter_t get_access_stamp() const {
			return m_access_counter;
		}

	private:
		std::list<Variable> m_data;
		std::string m_name;
		// Incremented when literal operators (user facing) are used 
		// to access this variable for disambiguation in certain cases
		stamp::counter_t m_access_counter;
	}; // class basic_variable_stack

	using variable_stack = generic_variable_stack<variable>;

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Variables                                                                 *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	class variable {
	public: // Lifecycle
		variable(variable_stack* owner)
			: m_data()
			, m_owner(owner)
			, m_active(false)
			, m_unpack_count(0)
			, m_previous_unpack_access_stamp(0)
			, m_previous_assignment_stamp(0)
		{
		}

		variable(variable const& other, variable_stack* owner)
			: m_data(other.m_data)
			, m_owner(owner)
			, m_active(other.m_active)
			, m_unpack_count(0)
			, m_previous_unpack_access_stamp(0)
			, m_previous_assignment_stamp(0) {
		}

		variable(variable&& other, variable_stack* owner) noexcept
			: m_data(std::move(other.m_data))
			, m_owner(owner)
			, m_active(std::exchange(other.m_active, false))
			, m_unpack_count(0)
			, m_previous_unpack_access_stamp(0)
			, m_previous_assignment_stamp(0) {
		}

		~variable() = default;

		inline variable_stack* get_owner() {
			return m_owner;
		}

	public: // Assignment Operators

		// Rebind variable name to the value of another variable
		inline variable& operator=(variable const& rhs) {
			if (this == &rhs) { return *this; }
			m_previous_assignment_stamp = get_access_stamp();
			m_data.assign(rhs.get_value());
			create();
			return *this;
		}

		/// TODO: rhs has to be passed by value otherwise T will be innacurate when const. Find a fix
		template<typename T>
		inline variable& operator=(T rhs) {
			m_previous_assignment_stamp = get_access_stamp();
			// Explicitly instantiate reflection for T 
			VIPER_INTERNAL_INSTANTIATE_TYPE(std::decay_t<T>);
			m_data.assign(value(rhs));
			create();
			return *this;
		}

	public: // Access Operator

		// To avoid issues with literal operator followed by .
		inline variable* operator->() {
			return this;
		}

	public: // Unpacking Operator

		inline variable& operator*() {
			const auto current_access_stamp = get_access_stamp();
			if (m_unpack_count == 0) {
				m_unpack_count = 1;
			} else if (m_unpack_count == 1) {
				// What we are checking for here is that the order of unpack operations looked like this:
				//     **"var_" = ...;
				// And not like this:
				//     *"var"_ = ...;
				//     *"var"_ = ...;
				if (not stamp::is_newer(current_access_stamp, m_previous_unpack_access_stamp)) {
					m_unpack_count = 2;
				}
			} else {
				m_unpack_count = 0;
				throw syntax_error("***variable is invalid syntax");
			}
			m_previous_unpack_access_stamp = current_access_stamp;
			return *this;
		}
			
	public: // Utility

		inline void reset_syntax_state() {
			(void)steal_last_assignment();
			(void)steal_unpack_count();
		}

		inline variable& direct_assign(value const& new_value) {
			m_data.assign(new_value);
			accept_last_assignment();
			create();
			return *this;
		}

		inline void accept_last_assignment() {
			m_data.accept_last_assignment();
		}

		inline value const& get_value() const {
			return m_data.get();
		}

		inline value& get_value() {
			return m_data.get();
		}

		inline uint8_t steal_unpack_count() {
			return std::exchange(m_unpack_count, uint8_t(0));
		}

		inline value steal_last_assignment() {
			return m_data.steal_last_assignment();
		}

		inline stamp::counter_t get_last_assignment_stamp() const {
			return m_previous_assignment_stamp;
		}

		inline stamp::counter_t get_access_stamp() const {
			return m_owner->get_access_stamp();
		}
		
		inline std::string const& get_name() const {
			return m_owner->get_name();
		}

	private: // Helpers

		inline void create() {
			m_active = true;
		}

		inline void destroy() {
			m_active = false;
			m_data.reset();
		}
			
	private: // Member Variables

		// Encapsulate the state of value pointers
		class data_state {
		public:
			data_state()
				: m_value(nullptr)
				, m_last_assignment(nullptr)
			{}

			inline void assign(value const& new_value) {
				accept_last_assignment();
				m_last_assignment.assign(new_value);
			}

			inline void accept_last_assignment() const {
				if (m_last_assignment.is_some()) {
					m_value.assign(m_last_assignment);
					m_last_assignment.reset();
				}
			}

			inline value const& get() const {
				accept_last_assignment();
				return m_value;
			}

			inline value& get() {
				accept_last_assignment();
				return m_value;
			}

			inline value steal_last_assignment() {
				value last_assignment(std::move(m_last_assignment));
				return last_assignment;
			}

			inline void reset() {
				m_value.reset();
				m_last_assignment.reset();
			}

		private:
			mutable value m_value;
			mutable value m_last_assignment;
		};

		data_state m_data;
		variable_stack* m_owner; // Invariant: Never null

		bool m_active;
		uint8_t m_unpack_count;
		stamp::counter_t m_previous_unpack_access_stamp;
		stamp::counter_t m_previous_assignment_stamp;
	}; // class variable

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

		inline variable_stack& get_stack(std::string const& name) {
			return m_data.try_emplace(name, name).first->second;
		}
		inline variable& get(std::string const& name) {
			return get_stack(name).top();
		}

		// Same as get but also increments the variable stack's access counter
		// Used by variable access literal operators
		inline variable& literal_access(std::string const& name) {
			variable_stack& stack = get_stack(name);
			stack.increment_access_count();
			return stack.top();
		}

		inline variable_stack* find(std::string const& name) {
			const auto it = m_data.find(name);
			return it != m_data.end() ? &(it->second) : nullptr;
		}

		inline variable_stack const* find(std::string const& name) const {
			const auto it = m_data.find(name);
			return it != m_data.end() ? &(it->second) : nullptr;
		}

	private:
		map_type m_data;
	}; // class variable_storage

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Variable Access Literal Operators                                         *
\*~-------------------------------------------------------------------------~*/

namespace viper_::literals {
	inline detail::variable& (operator""_)(const char* string, size_t length) {
		return detail::variable_storage::global_context().literal_access(std::string(string, length));
	}

	inline detail::variable& (operator""_)(uint64_t integer) {
		return detail::variable_storage::global_context().literal_access(std::to_string(integer));
	}

	inline detail::variable& (operator""_)(long double real) {
		return detail::variable_storage::global_context().literal_access(std::to_string(real));
	}

	inline detail::variable& (operator""_VIPER_UNDERSCORE)(const char* string, size_t length) {
		return detail::variable_storage::global_context().literal_access(std::string(string, length));
	}

	inline detail::variable& (operator""_VIPER_UNDERSCORE)(uint64_t integer) {
		return detail::variable_storage::global_context().literal_access(std::to_string(integer));
	}

	inline detail::variable& (operator""_VIPER_UNDERSCORE)(long double real) {
		return detail::variable_storage::global_context().literal_access(std::to_string(real));
	}
} // namespace viper_::literals

#if not defined(VIPER_NO_NAMESPACE_POLLUTION)
	using namespace viper_::literals;
#endif // not defined (VIPER_NO_NAMESPACE_POLLUTION)

/*~-------------------------------------------------------------------------~*\
 * Value Access Helper                                                       *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {

	inline value to_value(value value) {
		return value;
	}

	inline value to_value(variable const& variable) {
		return variable.get_value();
	}

	template<typename T>
	inline value to_value(T data) {
		return value(data);
	}

} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Collection Types                                                          *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {

	class list {
		using in_index_t = std::int64_t;
		using out_index_t = std::int64_t;
	public: // Lifecycle
		list()
			: m_values()
		{}
		
		template<typename... Ts>
		list(Ts&&... elements)
			: m_values()
		{
			(void)(sizeof(::viper_::detail::instantiate_type<std::decay_t<Ts>>), ...);

			m_values.reserve(sizeof...(Ts));
			(m_values.emplace_back(detail::to_value(std::forward<Ts>(elements))), ...);
		}

		list(list& other)
			: m_values(other.m_values) {
		}

		list(list const& other)
			: m_values(other.m_values)
		{
		}

		list(list&& other) noexcept
			: m_values(std::move(other.m_values))
		{
		}

	public: // Interface

		detail::value pop(in_index_t index = -1) {
			const size_t real_index = fix_index(index);
			if (not in_range(real_index)) {
				throw index_error("pop index out of range");
			}
			detail::value popped = m_values[real_index];
			m_values.erase(m_values.begin() + real_index);
			return popped;
		}

		template<typename T>
		void append(T&& element) {
			m_values.emplace_back(detail::to_value(std::forward<T>(element)));
		}

		template<typename T>
		out_index_t index(T const& element, in_index_t start = 0, in_index_t stop = -1) const {
			const size_t real_start_index = fix_index(start);
			if (not in_range(real_start_index)) {
				throw index_error("index start out of range");
			}
			const size_t real_stop_index = fix_index(stop);
			if (not in_range(real_stop_index - 1)) {
				throw index_error("index stop out of range");
			}

			const auto not_in_list_error = [&]() {
				std::string stringized_element = detail::string_representation<T>::get(element);
				if (stringized_element.empty()) {
					stringized_element = "[?]";
				}
				throw value_error(std::format("{} is not in list", stringized_element));
			};

			if (real_start_index >= real_stop_index) {
				not_in_list_error();
			}
			
			for (size_t i = real_start_index; i < real_stop_index; ++i) {
				// We make value::operator== do the work resolving between T and value
				if (m_values[i] == element) {
					return static_cast<out_index_t>(i);
				}
			}
			not_in_list_error();
			return -1;
		}

		void clear() {
			m_values.clear();
		}

		void extend(list const& iterable) {
			m_values.insert(m_values.end(), iterable.m_values.begin(), iterable.m_values.end());
		}

		void reverse() {
			std::reverse(m_values.begin(), m_values.end());
		}

		template<typename T>
		void insert(in_index_t index, T&& element) {
			size_t real_index = fix_index(index);
			if (not in_range(index)) {
				throw index_error("insert index out of range");
			}
			m_values.emplace(m_values.begin() + real_index, detail::to_value(std::forward<T>(element)));
		}

		/// TODO: Remove
		std::vector<detail::value> const& get_data() const {
			return m_values;
		}
		std::vector<detail::value>& get_data() {
			return m_values;
		}
		
	public: // Dunder Methods

		size_t __len__() const {
			return m_values.size();
		}

		std::string __str__() const {
			return __repr__();
		}

		std::string __repr__() const {
			std::string out = "[";
			for (size_t i = 0; i < m_values.size(); ++i) {
				out += detail::string_representation<detail::value>::get(m_values[i]);
				if (i + 1 != m_values.size()) {
					out += ", ";
				}
			}
			return out += "]";
		}

	private: // Helper Methods
		size_t fix_index(in_index_t index) const {
			return static_cast<size_t>(index >= 0 ? index : m_values.size() + index + 1);
		}

		bool in_range(size_t index) const {
			return index < m_values.size();
		}

	private:
		std::vector<detail::value> m_values;
	}; // class list

	class tuple {
	public:
		
	private:
	}; // class tuple

	class dict {
	public:
		
	private:
	}; // class dict

} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Functions                                                                 *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	class function {
	private: // Helper types
		enum class parameter_type : int8_t {
			positional = 0,
			positional_with_default,
			positional_catcher,
			keyword,
			keyword_with_default,
			keyword_catcher
		};

		struct parameter {
			variable_stack* variable;
			value default_value;
			parameter_type type;
			uint8_t unpack_count;
			bool assigned_by_call;
		};

		struct process_arguments_state {
			enum class argument_phase : int8_t {
				positional = 0,
				keyword
			} phase;
		};

	public: // Lifecycle
		using callable_type = std::function<value(function&)>;

		inline function(std::string&& name, std::vector<variable*> const& parameters, callable_type&& callable)
			: m_name(move(name))
			, m_parameters()
			, m_callable(move(callable))
			, m_positional_catcher_index(-1)
			, m_keyword_catcher_index(-1)
		{
			enum parameter_phase : int {
				positional = 0,
				positional_with_default,
				keyword_args,
				finished
			} phase = positional;

			/// TODO: check for duplicate parameter names in this function

			m_parameters.reserve(parameters.size());
			// We reset variable buffered states in this loop so that afterwards, 
			// exceptions can safely be thrown and they won't break variable states
			for (variable* parameter : parameters) {
				m_parameters.push_back({
					parameter->get_owner(),
					parameter->steal_last_assignment(),
					parameter_type::positional,
					parameter->steal_unpack_count(),
				});
				// Just in case we forget to steal something
				parameter->reset_syntax_state();
			}

			// This isn't included in the above for loop because throwing 
			// an exception before all input variables are reset causes issues
			for (int i = 0; i < int(m_parameters.size()); ++i) {
				if (m_parameters[i].variable == nullptr) {
					throw type_error(std::format("Parameter {} does not declare a parameter name", i));
				}
			}

			/// TODO: Provide more specifics in exceptions including parameter name and index
			for (size_t i = 0; i < parameters.size(); ++i) {
				parameter& parameter = m_parameters[i];

				// Tries to change the phase based on unpacks (*, **), returns true if it succeeds
				const auto unpack_change_phase = [&]() -> bool {
					if (parameter.unpack_count == 1) {
						if (phase >= keyword_args) {
							throw type_error("*arguments cannot appear more than once");
						} else if (parameter.default_value.is_some()) {
							throw type_error("**keyword arguments cannot have a default value");
						}
						m_positional_catcher_index = static_cast<int>(i);
						phase = keyword_args;
						parameter.type = parameter_type::positional_catcher;
						//parameter.default_value.assign(list());
						return true;
					} else if (parameter.unpack_count == 2) {
						if (parameter.default_value.is_some()) {
							throw type_error("*arguments cannot have a default value");
						}
						// Arguments after **kwargs error handled below in finished case of phase switch
						m_keyword_catcher_index = static_cast<int>(i);
						phase = finished;
						parameter.type = parameter_type::keyword_catcher;
						//parameter.default_value.assign(dict());
						return true;
					} else if (parameter.unpack_count >= 3) {
						throw type_error("Cannot put more than two '*' on an argument");
					}
					return false;
				};

				switch (phase) {
				case positional:
					if (not unpack_change_phase() and parameter.default_value.is_some()) {
						phase = positional_with_default;
						parameter.type = parameter_type::positional_with_default;
					}
					break;
				case positional_with_default:
					if (not unpack_change_phase()) {
						if (parameter.default_value.is_some()) {
							parameter.type = parameter_type::positional_with_default;
						} else {
							throw type_error("Argument without default value cannot follow arguments with default values");
						}
					}
					break;
				case keyword_args:
					if (not unpack_change_phase()) {
						if (parameter.default_value.is_some()) {
							parameter.type = parameter_type::keyword_with_default;
						} else {
							parameter.type = parameter_type::keyword;
						}
					}
					break;
				case finished:
					throw type_error("Additional arguments not allowed after **keyword arguments");
				}
			}
		}

		inline ~function() = default;

	public: // Calling

		template<class... Arguments>
		inline value operator()(Arguments&&... arguments) {
			// We assume at first that all arguments passed are valid, meaning all parameter variables will need to be pushed
			// Only wastes time in pushing variables if there is an exception in processing the arguments which is insignificant
			for (parameter& parameter : m_parameters) {
				parameter.variable->push();
				parameter.assigned_by_call = false;
				// Catchers can be assigned to multiple times by appending so they need to already exist before we process args
				if (parameter.type == parameter_type::positional_catcher) {
					parameter.variable->top().direct_assign(list());
				} else if (parameter.type == parameter_type::keyword_catcher) {
					parameter.variable->top().direct_assign(dict());

				}
			}

			// Declared as a lambda so it can be called in case process_arguments throws an error
			const auto reset_variables = [&]() {
				for (parameter const& parameter : m_parameters) {
					parameter.variable->pop();
				}
				((reset_passed_variable_syntax_state(std::forward<Arguments>(arguments))), ...);
			};
			// process_arguments with 0 Arguments won't compile
			if constexpr (sizeof...(Arguments) > 0) {
				try {
					// Initialize a mutable state for process_arguments to work with
					process_arguments_state state{
						.phase = process_arguments_state::argument_phase::positional,
					};
					process_arguments<0llu>(state, std::forward<Arguments>(arguments)...);
				} catch (...) {
					reset_variables();
					throw;
				}
			}
			
			// It's cleaner to check for missing arguments here than in process_arguments
			for (parameter const& parameter : m_parameters) {
				if (not parameter.assigned_by_call) {
					if (parameter.default_value.is_none() 
						and parameter.type != parameter_type::positional_catcher
						and parameter.type != parameter_type::keyword_catcher
					) {
						throw type_error(std::format("Required parameter {} not passed in call to function", parameter.variable->get_name()));
					} else {
						parameter.variable->top().direct_assign(parameter.default_value);
					}
				}
			}

			value return_value = m_callable(*this);
			reset_variables();
			return return_value;
		}

	private: // Helper Functions

		inline bool has_positional_catcher() const {
			return m_positional_catcher_index != -1;
		}
		inline bool has_keyword_catcher() const {
			return m_keyword_catcher_index != -1;
		}

		// Used by process_arguments to reset any remaining variable states in the event of an exception
		template<class T>
		inline constexpr void reset_passed_variable_syntax_state(T&& argument) {
			if constexpr (std::is_same_v<std::decay_t<T>, variable>) {
				argument.reset_syntax_state();
			}
		}

		inline void mark_parameter_assigned(parameter& parameter) {
			if (std::exchange(parameter.assigned_by_call, true)) {
				throw type_error(std::format("Parameter {} already assigned in call to function", parameter.variable->get_name()));
			}
		}

		template<size_t Index>
		inline void process_variable_argument(process_arguments_state& state, variable& argument) {
			using argument_phase = process_arguments_state::argument_phase;
			const auto keyword_matches_name = [&keyword = argument.get_name()](parameter const& parameter) {
				return parameter.variable->get_name() == keyword;
			};

			if (state.phase == argument_phase::positional) {
				const value assigned_value = argument.steal_last_assignment();
				if (assigned_value.is_some()) {
					if (stamp::is_newer(argument->get_access_stamp(), argument.get_last_assignment_stamp())) {
						argument->direct_assign(assigned_value);
						parameter& parameter = m_parameters[Index];
						mark_parameter_assigned(parameter);
						parameter.variable->top().direct_assign(argument.get_value());
					} else {
						state.phase = argument_phase::keyword;
						const auto matching_parameter = std::find_if(m_parameters.begin(), m_parameters.end(), keyword_matches_name);
						if (matching_parameter != m_parameters.end()) {
							mark_parameter_assigned(*matching_parameter);
							matching_parameter->variable->top().direct_assign(assigned_value);
						} else {
							if (has_keyword_catcher()) {
								/// TODO: Add key to keyword catcher dict here
								//m_parameters[m_keyword_catcher_index].variable->top().get_value().
							} else {
								throw type_error("Keyword argument does not name any parameters and function doesn't accept **keyword arguments");
							}
						}
					}
				} else /* assigned_value.is_none() */ {
					const parameter_type current_parameter_type = m_parameters[Index].type;
					if (current_parameter_type != parameter_type::positional and current_parameter_type != parameter_type::positional_with_default) {
						if (has_positional_catcher()) {
							/// TODO: Append to end of positional catcher tuple here
							list& positional_catcher = std::any_cast<list&>(m_parameters[m_positional_catcher_index].variable->top().get_value().data().get_data_storage());
							positional_catcher.append(argument.get_value());
						} else {
							throw type_error("Too many positional arguments and function doesn't accept *positional arguments");
						}
					} else {
						parameter& parameter = m_parameters[Index];
						mark_parameter_assigned(parameter);
						parameter.variable->top().direct_assign(argument.get_value());
					}
				}
			} else /* state.phase == argument_phase::keyword */ {
				const value assigned_value = argument.steal_last_assignment();
				if (assigned_value.is_some()) {
					if (stamp::is_newer(argument->get_access_stamp(), argument.get_last_assignment_stamp())) {
						argument->direct_assign(assigned_value);
						throw type_error("Cannot place positional arguments after keyword arguments");
					} else {
						const auto matching_parameter = std::find_if(m_parameters.begin(), m_parameters.end(), keyword_matches_name);
						if (matching_parameter != m_parameters.end()) {
							mark_parameter_assigned(*matching_parameter);
							matching_parameter->variable->top().direct_assign(assigned_value);
						} else {
							if (has_keyword_catcher()) {
								/// TODO: Add key to keyword catcher dict here
								//m_parameters[m_keyword_catcher_index].variable->top().get_value().
							} else {
								throw type_error("Keyword argument does not name any parameters and function doesn't accept **keyword arguments");
							}
						}
					}
				} else /* assigned_value.is_none() */ {
					throw type_error("Cannot place positional arguments after keyword arguments");
				}
			}
		}

		template<size_t Index, class T>
		inline void process_argument(process_arguments_state& state, T const& argument) {
			using argument_phase = process_arguments_state::argument_phase;
			if (state.phase == argument_phase::positional) {
				const parameter_type current_parameter_type = m_parameters[Index].type;
				if (current_parameter_type != parameter_type::positional and current_parameter_type != parameter_type::positional_with_default) {
					if (has_positional_catcher()) {
						/// TODO: Append to end of positional catcher tuple here
						//m_parameters[m_positional_catcher_index].variable->top().get_value().
						list& positional_catcher = std::any_cast<list&>(m_parameters[m_positional_catcher_index].variable->top().get_value().data().get_data_storage());
						positional_catcher.append(argument);
					} else {
						throw type_error("Too many positional arguments and function doesn't accept *positional arguments");
					}
				} else {
					parameter& parameter = m_parameters[Index];
					mark_parameter_assigned(parameter);
					parameter.variable->top().direct_assign(value(argument));
				}
			} else /* state.phase == argument_phase::keyword */ {
				throw type_error("Cannot place positional arguments after keyword arguments");
			}
		}

		template<size_t Index, class First, class... Rest>
		inline constexpr void process_arguments(process_arguments_state& state, First&& first, Rest&&... rest) {
			// Stop forwarding argument types here, we know we want mutable variables and immutable everything else
			if constexpr (std::is_same_v<std::decay_t<First>, variable>) {
				process_variable_argument<Index>(state, first);
			} else {
				process_argument<Index, First>(state, first);
			}
			if constexpr (sizeof...(Rest) > 0) {
				process_arguments<Index + 1>(state, std::forward<Rest>(rest)...);
			}
		}

	public: // Data Aliases
		std::string const& __name__ = m_name;

	private:
		std::string m_name;
		std::vector<parameter> m_parameters;
		callable_type m_callable;

		int m_positional_catcher_index;
		int m_keyword_catcher_index;
	}; // class function
} // namespace viper_::detail

/*~-------------------------------------------------------------------------~*\
 * Functions Helpers                                                         *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	// Helper class used in making a complete function object within the def macro
	class function_builder {
	public:
		inline function_builder(const char* name)
			: m_name(name)
			, m_parameters()
		{}

	private: // Helper Traits
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
					[&](function& function) -> value {
						return value(callable(function));
					}
				};
			} else {
				return { move(m_name), m_parameters,
					[&](function& function) -> value {
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
			} else if (inside_format and out[i] == '}') {
				inside_format = false;
				const int variable_length = i - begin_format - 1;

				detail::variable_storage const& variables = detail::variable_storage::global_context();
				detail::type_record_storage const& types = detail::type_record_storage::global_context();

				std::string data_string = "";

				if (variable_stack const* variable = variables.find(out.substr(begin_format + 1llu, variable_length))) {
					value const& variable_value = variable->top().get_value();
					if (variable_value.is_some()) {
						if (type_record const* type = types.find(variable_value.data().type().hash_code())) {
							data_string = type->get_string_data(variable->top().get_value().data().get_data_storage());
						}
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
 * Underscore "Operator"                                                     *
\*~-------------------------------------------------------------------------~*/

namespace viper_::detail {
	class underscore {
	public: 
		inline underscore() {

		}
	}; // class underscore_proxy
} // namespace viper_::detail

// Intentionally located in the global namespace so _ macro can name this, the literal operator,
// or the macro depending on if parenthesis are present after the identifier
static inline viper_::detail::underscore _VIPER_UNDERSCORE;

// Has a leading underscore so that the name can also be used for a user-defined literal operator
#define _VIPER_UNDERSCORE(Type) VIPER_HINT(Type)

/*~-------------------------------------------------------------------------~*\
 * Constants                                                                 *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {
	static inline constexpr bool True = true;
	static inline constexpr bool False = false;
	static inline const detail::value None{};
} // namespace viper_

/*~-------------------------------------------------------------------------~*\
 * Print Function                                                            *
\*~-------------------------------------------------------------------------~*/

namespace viper_ {
	inline def(print)(*"values"_, "sep"_ = " ", "end"_ = "\n", "flush"_ = False) {
		list const& positional_catcher = std::any_cast<list&>("values"_->get_value().data().get_data_storage());
		auto const& positional_arguments = positional_catcher.get_data();
		const size_t end = positional_arguments.size() - 1;
		
		const std::string separator = detail::string_representation<detail::value>::get("sep"_->get_value());
		const std::string end_token = detail::string_representation<detail::value>::get("end"_->get_value());
		for (size_t i = 0; i < end; ++i) {
			std::cout << detail::string_representation<detail::value>::get(positional_arguments[i]) << separator;
		}
		std::cout << detail::string_representation<detail::value>::get(positional_arguments[end]) << end_token;
		if ("flush"_->get_value().is_truthy()) {
			std::cout << std::flush;
		}
	};
} // namespace viper_


/*~-------------------------------------------------------------------------~*\
 * Type and Value Aliases                                                    *
\*~-------------------------------------------------------------------------~*/

#if not defined(VIPER_NO_NAMESPACE_POLLUTION)
	using viper_::print;

	using viper_::hint;
	using viper_::type_error;

	using viper_::True;
	using viper_::False;

	using viper_::list;
	using viper_::tuple;
#endif // not defined(VIPER_NO_NAMESPACE_POLLUTION)

/*~-------------------------------------------------------------------------~*\
 * Upcoming Features                                                         *
\*~-------------------------------------------------------------------------~*/

// import
// classes
// modules and __name__
// exec() with dlls
// help()
