#pragma once
#include <chrono>

namespace Crystal {
	class CrystalTimer {
	public:
		CrystalTimer() noexcept;

		void Tick() noexcept;
		void Reset() noexcept;
		void ResetElapsed() noexcept;

		template<typename T = std::ratio<1>>
		float GetDelta() const noexcept;

		template<typename T = std::ratio<1>>
		float GetElapsed() const noexcept;
	private:
		std::chrono::high_resolution_clock::time_point m_last;
		std::chrono::high_resolution_clock::duration m_delta{ 0 };
		std::chrono::high_resolution_clock::duration m_elapsed{ 0 };
	};

	template<typename T>
	float CrystalTimer::GetDelta() const noexcept {
		return std::chrono::duration<float, T>(m_delta).count();
	}

	template<typename T>
	float CrystalTimer::GetElapsed() const noexcept {
		return std::chrono::duration<float, T>(m_elapsed).count();
	}
}