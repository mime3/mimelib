#ifndef __MINLIB_LOGGER__
#define __MINLIB_LOGGER__

#pragma once
#define LOGGER			Logger::GetInstance()
#define LOG_DEBUG		Logger::LOG_LEVEL_DEBUG
#define LOG_WARNING		Logger::LOG_LEVEL_WARNING
#define LOG_ERROR		Logger::LOG_LEVEL_ERROR
#define LOG(filename, logLevel, fmt, ...)		LOGGER.Log(filename, logLevel, fmt, __VA_ARGS__)
#define LOGHEX(filename, logLevel, log, size)	LOGGER.LogHex(filename, logLevel, log, size)


#include <string>
#include <Windows.h>
#include <time.h>
#include <direct.h>
#include <strsafe.h>
#include <locale.h>
#include "Clock.h"

using namespace std;
// 로그 클래스
namespace MinLib
{
	class Logger
	{
	public:
		enum LOG_LEVEL
		{
			LOG_LEVEL_DEBUG,
			LOG_LEVEL_WARNING,
			LOG_LEVEL_ERROR
		};
	private:
		int _logLevel;					// 로그 레벨
		bool _fileSaveFlag;				// 파일저장 플래그s
		wstring _logDir;				// 로그저장 경로이름
		unsigned int _logCount;			// 로그 카운트
		static Logger* _instance;		// 싱글톤
		Logger();
		~Logger() {};
	public:
		static Logger& GetInstance()
		{
			if (_instance == nullptr)
				_instance = new Logger;
			return *_instance;
		}
		// 로그작성 멤버 함수
		void Log(const WCHAR* filename, LOG_LEVEL logLevel, const WCHAR* fmt, ...);
		void Log(const WCHAR* filename, const WCHAR* fmt, va_list args);
		// 16진수로 로그작성
		void LogHex(const WCHAR* filename, LOG_LEVEL logLevel, const CHAR* loghex, int hexSize);
		// 로그레벨 세팅
		void SetLogLevel(LOG_LEVEL level);
		// 로그디렉토리
		void SetLogDir(wstring* fileName);
		// 온오프
		void OnOff(bool onoff);
	};
}
#endif //__MINLIB_LOGGER__