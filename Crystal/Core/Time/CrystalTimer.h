#pragma once
#include <chrono>
#include <type_traits>
#include <cstdint>
#include "../Core/Lib/type_traits.h"

namespace Crystal {
	namespace detail {
		template<class T>
		constexpr bool is_duration_v = crylib::is_specialization_v<T, std::chrono::duration>;

		template<class T>
		constexpr bool is_ratio_v = false;

		template<std::intmax_t nx, std::intmax_t dx>
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

		void begin_measure() noexcept {
			m_begin = Clock::now().time_since_epoch();
		}

		template<class Duration = std::chrono::milliseconds>
		[[nodiscard]] auto end_measure() const noexcept ->
			std::enable_if_t<
				detail::is_duration_v<Duration>,
				Duration
			>
		{
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
			typename T = float,
			typename U = std::ratio<1>
		>
		[[nodiscard]] auto get_delta() const noexcept ->
			std::enable_if_t<
			    std::is_floating_point_v<T> && detail::is_ratio_v<U>,
				T
			>
		{
			return std::chrono::duration<T, U>(m_delta).count();
		}

		template<
			typename T = float,
			typename U = std::ratio<1>
		>
		[[nodiscard]] auto get_elapsed() const noexcept ->
			std::enable_if_t<
			    std::is_floating_point_v<T>&& detail::is_ratio_v<U>,
			    T
		    > 
		{
			return std::chrono::duration<T, U>(m_elapsed).count();
		}
	private:
		Clock::duration m_begin;
		Clock::duration m_last;
		Clock::duration m_delta{ 0 };
		Clock::duration m_elapsed{ 0 };
	};
}