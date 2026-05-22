#pragma once

#include <cstdint>

namespace Champ
{
	class Time
	{
	public:
		static void NewFrame();
		static float GetDeltaTime();
		static float GetElapsed();
		static double GetElapsedAsDouble();
		static double GetFPS();
		static double GetFrameTimeMilliseconds();
		static double GetFrameTimeMicroSeconds();
		static uint64_t GetFrameCount();
	};
}