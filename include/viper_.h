#pragma once

#include "common.h"
#include "variable_storage.h"
#include "literals.h"
#include "hint.h"

// Define VIPER_NO_NAMESPACE_POLLUTION before this file is
// included to avoid viper polluting the global namespace
#if !defined(VIPER_NO_NAMESPACE_POLLUTION)
using namespace viper_::literals;
using viper_::hint;
using viper_::type_error;
#endif // !defined(VIPER_NO_NAMESPACE_POLLUTION)
