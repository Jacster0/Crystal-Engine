#pragma once
#include <type_traits>

namespace Crystal::crylib {
	template<class, class, class = void>
	constexpr bool has_type_v = false;

	template<class T, class U>
	constexpr bool has_type_v<T, U, std::void_t<typename T::U>> = true;

	template<class, template<class...> class>
	constexpr bool is_specialization_v = false;

	template<template<class...> class Template, class... Args>
	constexpr bool is_specialization_v<Template<Args...>, Template> = true;
}