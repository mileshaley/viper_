#pragma once

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
