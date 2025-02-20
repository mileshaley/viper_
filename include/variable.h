#pragma once

#include <any> // variable data
#include <utility> // std::move

namespace viper_ {
	class variable {
	public:
		variable();
		variable(variable const&) = delete;
		variable(variable&&) = delete;

		variable& operator=(variable const& rhs) {

		}

		variable& operator=(variable&& rhs) noexcept {

		}

		template<typename T>
		variable& operator=(T const& rhs) {

		}

		template<typename T>
		variable& operator=(T&& rhs) noexcept {
			m_data = std::move(rhs);
		}

		void create();
		void destroy();

	private:
		std::any m_data;
		bool m_alive = false;

	}; // class variable
} // namespace viper_
