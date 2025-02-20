#pragma once

#include <any> // variable data
#include <utility> // std::move
#include "exception.h"

namespace viper_ {
	class variable {
	public:
		variable();
		variable(variable const&) = delete;
		variable(variable&&) = delete;

		variable& operator=(variable const& rhs) {
			if (this == &rhs) { return *this; }
			if (m_alive && m_data.type() != rhs.m_data.type()) {
				throw type_error("Variable type was reassigned");
			}
			m_data = rhs.m_data;
			create();
		}
		
		variable& operator=(variable&& rhs) noexcept {
			if (this == &rhs) { return *this; }
			if (m_alive && m_data.type() != rhs.m_data.type()) {
				throw type_error("Variable type was reassigned");
			}
			m_data = std::move(rhs.m_data);
			create();
		}
		
		template<typename T>
		variable& operator=(T const& rhs) {
			if (this == &rhs) { return *this; }
			if (m_alive && m_data.type() != typeid(T)) {
				throw type_error("Variable type was reassigned");
			}
			create();
		}
		
		template<typename T>
		variable& operator=(T&& rhs) noexcept {
			if (this == &rhs) { return *this; }
			if (m_alive && m_data.type() != typeid(T)) {
				throw type_error("Variable type was reassigned");
			}
			m_data = std::move(rhs);
			create();
		}

		void create();
		void destroy();

	private:
		std::any m_data;
		bool m_alive = false;

	}; // class variable
} // namespace viper_
