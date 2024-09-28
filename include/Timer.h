#pragma once
#include <chrono>
#include <MyUtils.h>



class Timer {
	using Clock_t = std::chrono::steady_clock;
	using TimePoint_t = Clock_t::time_point;
	using Milliseconds_t = std::chrono::milliseconds;

public:
	Timer() noexcept;
	explicit Timer(int msLimit) noexcept;

	void start() noexcept;
	[[nodiscard]] int getTimeLimit() const noexcept;
	[[nodiscard]] bool timeRanOut() const noexcept;
	[[nodiscard]] int timeElapsed() const noexcept;
	void setTimeLimit(int msLimit) noexcept;
	void pause() noexcept;

private:
	TimePoint_t m_startTime;
	TimePoint_t m_pauseTime;
	Milliseconds_t m_timeLimit = {};
	bool m_isPaused = false;

	[[nodiscard]] Milliseconds_t duration() const noexcept;
};
