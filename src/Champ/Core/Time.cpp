#include "Time.hpp"
#include <chrono>

namespace Champ
{
	typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> TimePoint;

	struct Timer
	{
		TimePoint tp1;
		TimePoint tp2;
		float deltaTime;
		double elapsedTime;

		Timer()
		{
			tp1 = std::chrono::system_clock::now();
			tp1 = std::chrono::system_clock::now();
			deltaTime = 0;
			elapsedTime = 0;
		}

		void Update()
		{
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsed = tp2 - tp1;
			tp1 = tp2;
			deltaTime = elapsed.count();
			elapsedTime += deltaTime;
		}
	};

	static Timer gTimer;
	static double gElapsedTime = 0.0;
	static double gAverageFPS = 0.0;
	static uint32_t gFPS = 0;
	static uint64_t gFrameCount = 0;

	void Time::NewFrame()
	{
		gTimer.Update();

		gElapsedTime += gTimer.deltaTime;
		gFPS++;

		if (gElapsedTime > 0.5)
		{
			gAverageFPS = gFPS / gElapsedTime;
			gFPS = 0;
			gElapsedTime = 0.0;
		}

		gFrameCount++;
	}

	float Time::GetDeltaTime()
	{
		return gTimer.deltaTime;
	}

	float Time::GetElapsed()
	{
		return static_cast<float>(gTimer.elapsedTime);
	}

	double Time::GetElapsedAsDouble()
	{
		return gTimer.elapsedTime;
	}

	double Time::GetFPS()
	{
		return gAverageFPS;
	}

	double Time::GetFrameTimeMilliseconds()
	{
		return gTimer.deltaTime * 1000.0;
	}

	double Time::GetFrameTimeMicroSeconds()
	{
		return gTimer.deltaTime * 1000000.0;
	}

	uint64_t Time::GetFrameCount()
	{
		return gFrameCount;
	}
}