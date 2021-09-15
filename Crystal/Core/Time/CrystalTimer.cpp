#include "CrystalTimer.h"

using namespace Crystal;

CrystalTimer::CrystalTimer() noexcept
	:
	m_last(std::chrono::high_resolution_clock::now())
{}

void Crystal::CrystalTimer::Tick() noexcept {
	const auto now = std::chrono::high_resolution_clock::now();
	m_delta = now - m_last;
	m_elapsed += m_delta;
	m_last = now;
}

void CrystalTimer::Reset() noexcept {
	m_last    = std::chrono::high_resolution_clock::now();
	m_delta   = std::chrono::high_resolution_clock::duration();
	m_elapsed = std::chrono::high_resolution_clock::duration();
}

void CrystalTimer::ResetElapsed() noexcept {
	m_elapsed = std::chrono::high_resolution_clock::duration();
}
