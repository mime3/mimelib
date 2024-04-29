#ifndef __MINLIB_LOGGER__
#define __MINLIB_LOGGER__

#pragma once
#include "SingleTon.h"

#include "Clock.h"
#include "LockFreeQueue.h"
#include "MemoryPool.h"

#include <string>
#include <Windows.h>

namespace MinLib
{
#define LOGGER			MinLib::Logger::GetInstance()
#define LOG_DEBUG		MinLib::Logger::LogLevel::LOG_LEVEL_DEBUG
#define LOG_WARNING		MinLib::Logger::LogLevel::LOG_LEVEL_WARNING
#define LOG_ERROR		MinLib::Logger::LogLevel::LOG_LEVEL_ERROR
#define LOG(filename, logLevel, fmt, ...)		LOGGER.Log(filename, logLevel, fmt, __VA_ARGS__)
#define LOGHEX(filename, logLevel, log, size)	LOGGER.LogHex(filename, logLevel, log, size)

	// 로그 클래스
	class Logger : public SingleTon<Logger>
	{
	public:
		enum class LogLevel
		{
			LOG_LEVEL_DEBUG,
			LOG_LEVEL_WARNING,
			LOG_LEVEL_ERROR
		};

		typedef struct LOG_MESSAGE
		{
			std::wstring messageString;
		} LogMessage;

	public:
		Logger();
		~Logger();

		// 로그
		void Log(const WCHAR* filename, LogLevel logLevel, const WCHAR* fmt, ...);
		void Log(const WCHAR* filename, const WCHAR* fmt, va_list args);
		// 16진수로 로그작성
		void LogHex(const WCHAR* filename, LogLevel logLevel, const CHAR* loghex, int hexSize);
		// 로그레벨 세팅
		void SetLogLevel(LogLevel level);
		// 로그디렉토리
		void SetLogDir(wstring* fileName);
		// 온오프
		void OnOff(bool onoff);
		//
		bool isFinalize();

	private:
		void initialize();
		static unsigned int __stdcall logThreadMain(LPVOID logger);

	private:
		LogLevel		logLevel_		= { LogLevel::LOG_LEVEL_DEBUG };	// 로그 레벨
		std::wstring	logDir_			= {};								// 로그저장 경로
		bool			fileSaveFlag_	= { false };						// 파일저장 플래그s
		unsigned int	logCount_		= { 0 };							// 로그 카운트
		bool			isFinalize_		= { false };


		HANDLE			logThread_			= { nullptr };	// 로그 스레드
		HANDLE			messageQueueEvent_	= { nullptr };	// 로그메시지 큐 이벤트
		LF_Queue<LogMessage*>		messageQueue_	= {};	// 로그메시지 큐
		MemoryPoolTLS<LogMessage>	messagePool_	= {};	// 로그메시지 블록 메모리풀
	};

}
#endif //__MINLIB_LOGGER__