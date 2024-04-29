#pragma once
#ifndef __MINLIB_CLOCK__
#define __MINLIB_CLOCK__
#include <Windows.h>
#include <string>
#include <time.h>
#define CLOCK Clock::GetInstance()
using namespace std;

namespace MinLib
{

	class Clock
	{
		static Clock* _instance;
		time_t _tick;
	private:
		Clock() { _tick = 0; };
		~Clock() {};
	public:
		static Clock& GetInstance()
		{
			if (_instance == nullptr)
				_instance = new Clock();
			return *_instance;
		}
		void GetTimeString(wstring* timeStr);
	};
}
#endif