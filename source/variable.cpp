#include "variable.h"


viper_::variable::variable() 
	: m_data()
	, m_alive(false)
{
}

void viper_::variable::create() {
	m_alive = true;
}

void viper_::variable::destroy() {

}
