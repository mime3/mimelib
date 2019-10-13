//#include "pch.h"
#include "Logger.h"

Logger::Logger()
{
	_logLevel = LOG_LEVEL_DEBUG;
	_fileSaveFlag = false;
	_logCount = 0;
	setlocale(LC_ALL, "korean");
}

void Logger::Log(const WCHAR * filename, LOG_LEVEL logLevel, const WCHAR * fmt, ...)
{
	if (_logLevel <= logLevel)
	{
		va_list args;
		va_start(args, fmt);
		this->Log(filename, fmt, args);
		va_end(args);
	}
}

void Logger::Log(const WCHAR * filename, const WCHAR * fmt, va_list args)
{
	WCHAR logStr[1024];
	//vswprintf_s(logStr, 1024, fmt, args);
	// stringcdprint
	StringCchVPrintf(logStr, 1024, fmt, args);
	// logMessage 생성시 시간 스트링으로 초기화 가능
	//wstring logMessage = L"["; 
	//logMessage += filename;
	//logMessage += L"] "; 
	wstring logMessage;

	CLOCK.GetTimeString(&logMessage);

	WCHAR countstring[64];
	int lockcount = InterlockedIncrement(&_logCount);
	_itow_s(lockcount, countstring, 10);
	logMessage += L"[";
	logMessage += countstring;
	logMessage += L"] ";

	logMessage += logStr;
	logMessage += L"\n";

	//wprintf_s(logMessage.c_str());

	if (_fileSaveFlag)
	{
		FILE *file;
		wstring fileFullPath;
		fileFullPath += _logDir;
		fileFullPath += L"/";
		fileFullPath += filename;
		fileFullPath += L".txt";
		while (_wfopen_s(&file, fileFullPath.data(), L"at+, ccs = UNICODE") != 0)
		{
			Sleep(0);
		}
		fwprintf_s(file, L"%ws", logMessage.c_str());		
		fclose(file);
		
	}
}

void Logger::LogHex(const WCHAR * filename, LOG_LEVEL logLevel, const CHAR * loghex, int hexSize)
{
	wstring logStr;
	for (int i = 0; i < hexSize; ++i)
	{
		WCHAR ByteHex[8];
		swprintf_s(ByteHex, 8, L"%02x", *(unsigned char *)(loghex + i));
		logStr += ByteHex;
	}

	wstring logMessage = L"[";
	logMessage += filename;
	logMessage += L"] ";

	CLOCK.GetTimeString(&logMessage);

	WCHAR countstring[64];
	int lockcount = InterlockedIncrement(&_logCount);
	_itow_s(lockcount, countstring, 10);
	logMessage += L"[";
	logMessage += countstring;
	logMessage += L"] ";

	logMessage += logStr;
	logMessage += L"\n";

	//wprintf_s(logMessage.c_str());

	if (_fileSaveFlag)
	{
		FILE *file;
		wstring fileFullPath;
		fileFullPath += _logDir;
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

void Logger::SetLogLevel(LOG_LEVEL level)
{
	_logLevel = level;
}

void Logger::SetLogDir(wstring * logdir)
{
	_wmkdir(logdir->c_str());
	_logDir = logdir->c_str();
	_fileSaveFlag = true;
}

void Logger::OnOff(bool onoff)
{
	_fileSaveFlag = onoff;
}
Logger * Logger::_instance = nullptr;