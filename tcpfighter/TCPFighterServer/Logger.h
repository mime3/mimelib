#pragma once
#define LOG(logLevel, fmt, ...) Logger::GetInstance().Log(logLevel, fmt, __VA_ARGS__)
// �α� Ŭ����
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
	int _logLevel;					// �α� ����
	bool _fileSaveFlag;				// �������� �÷���
	wstring * _fileName;
	static Logger * _instance;		// �̱���
	Logger();						
	~Logger() {};
public:
	static Logger & GetInstance() 
	{
		if (_instance == nullptr)
			_instance = new Logger;
		return *_instance;
	}
	// �α��ۼ� ��� �Լ�
	void Log(LOG_LEVEL logLevel, const WCHAR * fmt, ...);
	void Log(const WCHAR *fmt, va_list args);
	// �α׷��� ����
	void SetLogLevel(LOG_LEVEL level);
	// �����̸�
	void SetFileName(wstring * fileName);
	// �¿���
	void OnOff(bool onoff);
};