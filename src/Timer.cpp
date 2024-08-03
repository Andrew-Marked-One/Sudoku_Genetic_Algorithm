#include "Timer.h"


Timer::Timer() {}

Timer::Timer(int msLimit)
	: m_timeLimit(msLimit) {}

void Timer::start() {
	m_startTime = Clock::now();
}

bool Timer::timeRanOut() const {
	if (m_timeLimit == Milliseconds()) {
		return false;
	}

	return duration() > m_timeLimit;
}

int Timer::timeElapsed() const {
	return static_cast<int>(duration().count());
}

void Timer::setTimeLimit(int msLimit) {
	m_timeLimit = Milliseconds(msLimit);
}

void Timer::pause() {
	if (m_isPaused) {
		m_startTime += std::chrono::duration_cast<Milliseconds>(Clock::now() - m_pauseTime);
	}
	else {
		m_pauseTime = Clock::now();
	}

	m_isPaused = !m_isPaused;
}

Timer::Milliseconds Timer::duration() const {
	Milliseconds duration = {};
	if (m_isPaused) {
		duration = std::chrono::duration_cast<Milliseconds>(m_pauseTime - m_startTime);
	}
	else {
		duration = std::chrono::duration_cast<Milliseconds>(Clock::now() - m_startTime);
	}
	return duration;
}
