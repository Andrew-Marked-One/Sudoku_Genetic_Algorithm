#include "Timer.h"


Timer::Timer() noexcept {
	start();
}

Timer::Timer(int msLimit) noexcept
	: m_timeLimit(msLimit) {
	
	INPUT_VALIDITY(msLimit >= 0);

	start();
}

void Timer::start() noexcept {
	m_startTime = Clock_t::now();
}

int Timer::getTimeLimit() const noexcept {
	return static_cast<int>(m_timeLimit.count());
}

bool Timer::timeRanOut() const noexcept {
	if (m_timeLimit == Milliseconds_t()) {
		return true;
	}

	return duration() >= m_timeLimit;
}

int Timer::timeElapsed() const noexcept {
	return static_cast<int>(duration().count());
}

void Timer::setTimeLimit(int msLimit) noexcept {
	INPUT_VALIDITY(msLimit >= 0);

	m_timeLimit = Milliseconds_t(msLimit);
}

void Timer::pause() noexcept {
	if (m_isPaused) {
		m_startTime += std::chrono::duration_cast<Milliseconds_t>(Clock_t::now() - m_pauseTime);
	}
	else {
		m_pauseTime = Clock_t::now();
	}

	m_isPaused = !m_isPaused;
}

Timer::Milliseconds_t Timer::duration() const noexcept {
	if (m_isPaused) {
		return std::chrono::duration_cast<Milliseconds_t>(m_pauseTime - m_startTime);
	}
	else {
		return std::chrono::duration_cast<Milliseconds_t>(Clock_t::now() - m_startTime);
	}
}
