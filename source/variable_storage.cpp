#include "variable_storage.h"

// static
viper_::detail::variable_storage& viper_::detail::variable_storage::global_context() {
	static variable_storage storage{};
	return storage;
}

viper_::detail::variable_storage::variable_storage()
	: m_data()
{
}

viper_::detail::variable_storage::map_type& viper_::detail::variable_storage::map() {
	return m_data;
}
