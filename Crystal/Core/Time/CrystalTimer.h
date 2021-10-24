#pragma once
#include <chrono>
#include <type_traits>
#include <cstdint>

namespace Crystal {
	namespace detail {
		template<class, template<class...> class>
		constexpr bool is_specialization_v = false;

		template<template<class...> class Template, class... Args>
		constexpr bool is_specialization_v<Template<Args...>, Template> = true;

		template<class T>
		constexpr bool is_duration_v = is_specialization_v<T, std::chrono::duration>;

		template<class T>
		constexpr bool is_ratio_v = false;

		template <intmax_t nx, intmax_t dx>
		constexpr bool is_ratio_v<std::ratio<nx, dx>> = true;

		template<class, class = void>
		constexpr bool is_clock_v = false;

		template<class T>
		constexpr bool is_clock_v<T, std::void_t<
			typename T::duration,
			typename T::time_point,
			decltype(T::now())>> = true;
	}

	template<class Clock = std::chrono::high_resolution_clock>
	class CrystalTimer {
	public:
		static_assert(detail::is_clock_v<Clock>, "The specified clock did not meet the required implementation");

		CrystalTimer() noexcept 
			:
			m_last(Clock::now())
		{}

		void begin_mesure() noexcept {
			m_begin = Clock::now().time_since_epoch();
		}

		template<
			class Duration = std::chrono::milliseconds,
			typename       = std::enable_if_t<detail::is_duration_v<Duration>>
		>
		Duration end_measure() const noexcept {
			const auto end = Clock::now().time_since_epoch();
			const auto dt  = end - m_begin;

			return std::chrono::duration_cast<Duration>(dt);
		}

		void tick() noexcept {
			const auto now = Clock::now();
			m_delta        = now - m_last;
			m_last         = now;
			m_elapsed += m_delta;
		}

		void reset() noexcept {
			m_last    = Clock::now();
			m_delta   = Clock::duration();
			m_elapsed = Clock::duration();
		}
		void reset_elapsed() noexcept {
			m_elapsed = Clock::duration();
		}

		template<
			typename T = std::ratio<1>,
			typename   = std::enable_if_t<detail::is_ratio_v<T>>
		>
		float get_delta() const noexcept {
			return std::chrono::duration<float, T>(m_delta).count();
		}

		template<
			typename T = std::ratio<1>,
			typename   = std::enable_if_t<detail::is_ratio_v<T>>
		>
		float get_elapsed() const noexcept {
			return std::chrono::duration<float, T>(m_elapsed).count();
		}
	private:
		Clock::time_point m_begin;
		Clock::time_point m_last;
		Clock::duration m_delta{ 0 };
		Clock::duration m_elapsed{ 0 };
	};
}