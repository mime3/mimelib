#include "stdafx.h"

Logger::Logger()
{
	_logLevel = LOG_LEVEL_DEBUG;
	_fileSaveFlag = false;
	_fileName = nullptr;
}

void Logger::Log(LOG_LEVEL logLevel, const WCHAR * fmt, ...)
{
	if (_logLevel <= logLevel)
	{
		va_list args;
		va_start(args, fmt);
		this->Log(fmt, args);
		va_end(args);
	}
}

void Logger::Log(const WCHAR * fmt, va_list args)
{
	WCHAR logStr[1024];
	vswprintf_s(logStr, 1024, fmt, args);
	// logMessage 생성시 시간 스트링으로 초기화 가능
	wstring logMessage; 
	CLOCK.GetTimeString(&logMessage);
	logMessage += logStr;
	logMessage += L"\n";
	wprintf_s(logMessage.c_str());
	if (_fileSaveFlag)
	{
		FILE *file;
		if (_fileName == nullptr)
			return;
		if (_wfopen_s(&file, _fileName->data(), L"at+, ccs = UNICODE") == 0)
		{
			fwprintf_s(file, L"%ws", logMessage.c_str());		
			fclose(file);
		}
	}
}
void Logger::SetLogLevel(LOG_LEVEL level)
{
	_logLevel = level;
}
void Logger::SetFileName(wstring * fileName)
{
	_fileName = fileName;
	_fileSaveFlag = true;
}
void Logger::OnOff(bool onoff)
{
	_fileSaveFlag = onoff;
}
Logger * Logger::_instance = nullptr;