//#include "pch.h"
#include "Clock.h"

void Clock::GetTimeString(wstring * timeStr)
{
	tm * now = new tm;
	time(&_tick);
	localtime_s(now, &_tick);
	WCHAR buffer[34];
	swprintf_s(buffer, 34, L"%04d-%02d-%02d %02d:%02d:%02d :: ",
		now->tm_year + 1900,   // tm_year는 1900을 더해야 서기 연도가 됨
		now->tm_mon + 1, // tm_mon은 1월이 0, 2월이 1... 식으로 되어 있음 
		now->tm_mday,
		now->tm_hour,
		now->tm_min,
		now->tm_sec);
	*timeStr += buffer;
	delete now;
}
Clock * Clock::_instance = nullptr;
