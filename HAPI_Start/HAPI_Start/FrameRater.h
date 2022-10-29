#pragma once
#include <chrono>
#include <iostream>
#include <ratio>
#include <thread>

template<std::intmax_t FPS>
class FrameRater
{
public:
	FrameRater() :
		time_between_frames{ 1 }, tp{std::chrono::steady_clock::now()}
	{}

	void sleep() 
	{
		tp += time_between_frames;
		std::this_thread::sleep_until(tp);
	}

private:
	std::chrono::duration<double, std::ratio<1, FPS>> time_between_frames;
	std::chrono::time_point<std::chrono::steady_clock, decltype(time_between_frames)> tp;

};

