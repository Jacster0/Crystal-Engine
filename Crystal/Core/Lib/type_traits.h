#pragma once
#include <type_traits>

namespace Crystal::crylib {
	template<class T, class U, class = void>
	struct has_type : std::false_type {};

	template<class T, class U>
	struct has_type<T, U, std::void_t<typename T::U>> : std::true_type {};

	template<class T, class U>
	inline constexpr bool has_type_v = HasType<T, U>::value;

	template<class, template<class...> class>
	constexpr bool is_specialization_v = false;

	template<template<class...> class Template, class... Args>
	constexpr bool is_specialization_v<Template<Args...>, Template> = true;
}