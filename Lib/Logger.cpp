//#include "pch.h"
#include "Logger.h"

#include <time.h>
#include <direct.h>
#include <strsafe.h>
#include <locale.h>
#include <process.h>

namespace MinLib
{
	Logger::Logger()
	{
		initialize();
	}

	Logger::~Logger()
	{
		CloseHandle(messageQueueEvent_);
		isFinalize_ = true;
		WaitForSingleObject(logThread_, INFINITE);
		CloseHandle(logThread_);
	}

	void Logger::Log(const WCHAR* filename, LogLevel logLevel, const WCHAR* fmt, ...)
	{
		if (logLevel_ <= logLevel)
		{
			va_list args;
			va_start(args, fmt);
			this->Log(filename, fmt, args);
			va_end(args);
		}
	}

	void Logger::Log(const WCHAR* filename, const WCHAR* fmt, va_list args)
	{
		WCHAR logStr[1024];
		//vswprintf_s(logStr, 1024, fmt, args);
		// stringcdprint
		StringCchVPrintf(logStr, 1024, fmt, args);
		// logMessage 생성시 시간 스트링으로 초기화 가능
		//wstring logMessage = L"["; 
		//logMessage += filename;
		//logMessage += L"] "; 
		std::wstring logMessage;

		CLOCK.GetTimeString(&logMessage);

		WCHAR countstring[64];
		int lockcount = InterlockedIncrement(&logCount_);
		_itow_s(lockcount, countstring, 10);
		logMessage += L"[";
		logMessage += countstring;
		logMessage += L"] ";

		logMessage += logStr;
		logMessage += L"\n";

		//wprintf_s(logMessage.c_str());

		if (fileSaveFlag_)
		{
			LogMessage* newLog = messagePool_.Alloc();
			newLog->messageString = logMessage;
			messageQueue_.EnQueue(newLog);
			return;

			FILE* file;
			std::wstring fileFullPath;
			fileFullPath += logDir_;
			fileFullPath += L"/";
			fileFullPath += filename;
			fileFullPath += L".txt";
			while (_wfopen_s(&file, fileFullPath.data(), L"at+, ccs = UNICODE") != 0)
			{
				YieldProcessor();
				Sleep(0);
			}
			fwprintf_s(file, L"%ws", logMessage.c_str());
			fclose(file);
		}
	}

	void Logger::LogHex(const WCHAR* filename, LogLevel logLevel, const CHAR* loghex, int hexSize)
	{
		wstring logStr;
		for (int i = 0; i < hexSize; ++i)
		{
			WCHAR ByteHex[8];
			swprintf_s(ByteHex, 8, L"%02x", *(unsigned char*)(loghex + i));
			logStr += ByteHex;
		}

		std::wstring logMessage = L"[";
		logMessage += filename;
		logMessage += L"] ";

		CLOCK.GetTimeString(&logMessage);

		WCHAR countstring[64];
		int lockcount = InterlockedIncrement(&logCount_);
		_itow_s(lockcount, countstring, 10);
		logMessage += L"[";
		logMessage += countstring;
		logMessage += L"] ";

		logMessage += logStr;
		logMessage += L"\n";

		//wprintf_s(logMessage.c_str());

		if (fileSaveFlag_)
		{
			LogMessage* newLog = messagePool_.Alloc();
			newLog->messageString = logMessage;
			messageQueue_.EnQueue(newLog);
			return;

			FILE* file;
			std::wstring fileFullPath;
			fileFullPath += logDir_;
			fileFullPath += L"/";
			fileFullPath += filename;
			fileFullPath += L".txt";
			//if (_wfopen_s(&file, _fileName->data(), L"at+, ccs = UNICODE") == 0)
			if (_wfopen_s(&file, fileFullPath.data(), L"at+, ccs = UNICODE") == 0)
			{
				fwprintf_s(file, L"%ws", logMessage.c_str());
				fclose(file);
			}
		}
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		logLevel_ = level;
	}

	void Logger::SetLogDir(std::wstring* logdir)
	{
		_wmkdir(logdir->c_str());
		logDir_ = logdir->c_str();
		fileSaveFlag_ = true;
	}

	void Logger::OnOff(bool onoff)
	{
		fileSaveFlag_ = onoff;
	}

	bool Logger::isFinalize()
	{
		return isFinalize_;
	}

	void Logger::initialize()
	{
		setlocale(LC_ALL, "korean");

		messageQueueEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
		logThread_ = (HANDLE)_beginthreadex(NULL, NULL, logThreadMain, this, NULL, NULL);
	}

	unsigned int Logger::logThreadMain(LPVOID logger)
	{
		Logger* this_ = (Logger*)logger;
		std::wstring logString;
		while (true)
		{
			if (this_->isFinalize())
				break;

			WaitForSingleObject(this_->messageQueueEvent_, 10 * 1000);

			FILE* file = nullptr;
			std::wstring fileFullPath;
			fileFullPath += this_->logDir_;
			fileFullPath += L"/Log.txt";
			if (_wfopen_s(&file, fileFullPath.data(), L"at+, ccs = UNICODE") == 0)
				continue;

			LogMessage* logMessage = nullptr;
			while (this_->messageQueue_.DeQueue(&logMessage))
			{
				logString += logMessage->messageString;
			}

			fwprintf_s(file, L"%ws", logString.c_str());
			fclose(file);
			logString.clear();
		}
		return 0;
	}
}