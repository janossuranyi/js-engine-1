#ifndef JSE_TIMER_H
#define JSE_TIMER_H

#include "system/SystemTypes.hpp"
#include "system/Logger.hpp"
#include <chrono>

namespace jse {


	class SimpleTimer
	{

	public:
		SimpleTimer()
		{
			Reset();
		}

		inline void Reset() { start = std::chrono::steady_clock::now(); }
		inline void PrintElapsedTime(const String& aMsg)
		{
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed = end - start;

			Info("%s %fms", aMsg.c_str(), 1000 * elapsed.count());
		}
	private:
		std::chrono::steady_clock::time_point start;
	};

}
#endif