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

	// �α� Ŭ����
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

		// �α�
		void Log(const WCHAR* filename, LogLevel logLevel, const WCHAR* fmt, ...);
		void Log(const WCHAR* filename, const WCHAR* fmt, va_list args);
		// 16������ �α��ۼ�
		void LogHex(const WCHAR* filename, LogLevel logLevel, const CHAR* loghex, int hexSize);
		// �α׷��� ����
		void SetLogLevel(LogLevel level);
		// �α׵��丮
		void SetLogDir(wstring* fileName);
		// �¿���
		void OnOff(bool onoff);
		//
		bool isFinalize();

	private:
		void initialize();
		static unsigned int __stdcall logThreadMain(LPVOID logger);

	private:
		LogLevel		logLevel_		= { LogLevel::LOG_LEVEL_DEBUG };	// �α� ����
		std::wstring	logDir_			= {};								// �α����� ���
		bool			fileSaveFlag_	= { false };						// �������� �÷���s
		unsigned int	logCount_		= { 0 };							// �α� ī��Ʈ
		bool			isFinalize_		= { false };


		HANDLE			logThread_			= { nullptr };	// �α� ������
		HANDLE			messageQueueEvent_	= { nullptr };	// �α׸޽��� ť �̺�Ʈ
		LF_Queue<LogMessage*>		messageQueue_	= {};	// �α׸޽��� ť
		MemoryPoolTLS<LogMessage>	messagePool_	= {};	// �α׸޽��� ��� �޸�Ǯ
	};

}
#endif //__MINLIB_LOGGER__