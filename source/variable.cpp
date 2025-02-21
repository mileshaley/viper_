#include "variable.h"


viper_::detail::variable::variable() 
	: m_data()
	, m_alive(false)
	, m_hint()
{
}

void viper_::detail::variable::create() {
	m_alive = true;
}

void viper_::detail::variable::destroy() {
	m_alive = false;
	m_hint = nullptr;
	m_data.reset();
}

void viper_::detail::variable::check_assignment_type(std::type_info const& new_type) {
	if (m_alive) {
		if (m_data.type() != new_type) {
			throw type_error("Variable type was reassigned");
		}
	} else /* !m_alive */ {
		if (*m_hint != new_type) {
			throw type_error("Variable type does not match hint type");
		}
	}
}
