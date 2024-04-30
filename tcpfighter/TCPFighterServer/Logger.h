#pragma once
#include <string>

#define LOG(logLevel, fmt, ...) Logger::GetInstance().Log(logLevel, fmt, __VA_ARGS__)
// 로그 클래스
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
	bool _fileSaveFlag;				// 파일저장 플래그
	wstring * _fileName;
	static Logger * _instance;		// 싱글톤
	Logger();						
	~Logger() {};
public:
	static Logger & GetInstance() 
	{
		if (_instance == nullptr)
			_instance = new Logger;
		return *_instance;
	}
	// 로그작성 멤버 함수
	void Log(LOG_LEVEL logLevel, const WCHAR * fmt, ...);
	void Log(const WCHAR *fmt, va_list args);
	// 로그레벨 세팅
	void SetLogLevel(LOG_LEVEL level);
	// 파일이름
	void SetFileName(wstring * fileName);
	// 온오프
	void OnOff(bool onoff);
};