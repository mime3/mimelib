//#include "pch.h"
#include "DBConnector.h"
namespace MinLib
{
	extern CrashDump cd;

	DBConnector::DBConnector(const char* config)
	{
		char DBIP[16];
		char DBUser[64];
		char DBPassword[64];
		char DBName[64];
		_configParser.OpenFile(config);
		_configParser.GetValue_Str("DBIP", DBIP, "DB");
		_configParser.GetValue_Str("USER", DBUser, "DB");
		_configParser.GetValue_Str("PASSWORD", DBPassword, "DB");
		_configParser.GetValue_Str("DBNAME", DBName, "DB");
		_configParser.GetValue_Int("PORT", _DBPort, "DB");
		MultiByteToWideChar(CP_UTF8, NULL, DBIP, 16, _DBIP, 16);
		MultiByteToWideChar(CP_UTF8, NULL, DBUser, 64, _DBUser, 64);
		MultiByteToWideChar(CP_UTF8, NULL, DBPassword, 64, _DBPassword, 64);
		MultiByteToWideChar(CP_UTF8, NULL, DBName, 64, _DBName, 64);

		mysql_init(&_MySQL);

		Connect();
	}

	DBConnector::DBConnector(WCHAR* DBIP, WCHAR* User, WCHAR* Password, WCHAR* DBName, int DBPort)
	{
		StringCchCopy(_DBIP, 16, DBIP);
		StringCchCopy(_DBUser, 64, User);
		StringCchCopy(_DBPassword, 64, Password);
		StringCchCopy(_DBName, 64, DBName);
		_DBPort = DBPort;

		mysql_init(&_MySQL);

		Connect();
	}

	DBConnector::~DBConnector()
	{
		Disconnect();
	}

	bool DBConnector::Connect(void)
	{
		char DBIP_UTF8[16];
		char DBUser_UTF8[64];
		char DBPassword_UTF8[64];
		char DBName_UTF8[64];
		WideCharToMultiByte(CP_UTF8, NULL, _DBIP, 16, DBIP_UTF8, 16, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, NULL, _DBUser, 64, DBUser_UTF8, 64, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, NULL, _DBPassword, 64, DBPassword_UTF8, 64, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, NULL, _DBName, 64, DBName_UTF8, 64, NULL, NULL);
		_MySQLPointer = mysql_real_connect(&_MySQL, DBIP_UTF8, DBUser_UTF8, DBPassword_UTF8, DBName_UTF8, _DBPort, (char*)NULL, 0);
		if (_MySQLPointer == nullptr)
		{
			//fprintf(stderr, "Mysql connection error : %s", mysql_error(&_MySQL));
			SaveLastError();
			LOG(L"DB", LOG_ERROR, L"Mysql connection error : %s", _LastErrorMsg);
			return false;
		}

		mysql_set_character_set(_MySQLPointer, "utf8");

		return true;
	}

	bool DBConnector::Disconnect(void)
	{
		if (_MySQLPointer == nullptr)
			return false;
		mysql_close(_MySQLPointer);
		return true;
	}

	bool DBConnector::Query(const WCHAR* StringFormat, va_list args)
	{
		StringCchVPrintf(_Query, eQUERY_MAX_LEN, StringFormat, args);
		WideCharToMultiByte(CP_UTF8, NULL, _Query, -1, _QueryUTF8, eQUERY_MAX_LEN, NULL, NULL);

		int loop = 5;

		while (mysql_query(_MySQLPointer, _QueryUTF8) != 0)
		{
			SaveLastError();
			if (_LastError == CR_SOCKET_CREATE_ERROR ||
				_LastError == CR_CONNECTION_ERROR ||
				_LastError == CR_CONN_HOST_ERROR ||
				_LastError == CR_SERVER_GONE_ERROR ||
				_LastError == CR_SERVER_HANDSHAKE_ERR ||
				_LastError == CR_SERVER_LOST ||
				_LastError == CR_INVALID_CONN_HANDLE)
			{
				Connect();
				if (!loop--)
				{
					LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"쿼리 실패 %s", _Query);
					return false;
				}
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_WARNING, L"쿼리중 DB연결 오류 %d 회", 5 - loop);
				Sleep(0);
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"쿼리 실패 %s", _Query);
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"error No %d, msg : %s", _LastError, _LastErrorMsg);
				return false;
			}
		}

		return true;
	}

	bool DBConnector::Query(const WCHAR* StringFormat, ...)
	{
		va_list args;
		va_start(args, StringFormat);
		bool ret = Query(StringFormat, args);
		va_end(args);

		_SQLResult = mysql_store_result(_MySQLPointer);

		return ret;
	}

	bool DBConnector::Query_Save(const WCHAR* StringFormat, ...)
	{
		va_list args;
		va_start(args, StringFormat);
		bool ret = Query(StringFormat, args);
		va_end(args);
		return ret;
	}

	MYSQL_ROW DBConnector::FetchRow(void)
	{
		if (_SQLResult == NULL)
			return NULL;
		return mysql_fetch_row(_SQLResult);
	}

	void DBConnector::FreeResult(void)
	{
		mysql_free_result(_SQLResult);
	}

	void DBConnector::SaveLastError(void)
	{
		_LastError = mysql_errno(_MySQLPointer);
		const char* msgPointer = mysql_error(_MySQLPointer);
		MultiByteToWideChar(CP_UTF8, NULL, msgPointer, (int)strlen(msgPointer), _LastErrorMsg, 128);
	}

	////////////////////////////////////////////////////////////////

	void DBConnectorTLS::SaveLastError(void)
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return;
			}
		}

		connector->SaveLastError();
	}

	DBConnectorTLS::DBConnectorTLS(const char* config)
	{
		char DBIP[16];
		char DBUser[64];
		char DBPassword[64];
		char DBName[64];
		_configParser.OpenFile(config);
		_configParser.GetValue_Str("DBIP", DBIP, "DB");
		_configParser.GetValue_Str("USER", DBUser, "DB");
		_configParser.GetValue_Str("PASSWORD", DBPassword, "DB");
		_configParser.GetValue_Str("DBNAME", DBName, "DB");
		_configParser.GetValue_Int("PORT", _DBPort, "DB");
		MultiByteToWideChar(CP_UTF8, NULL, DBIP, 16, _DBIP, 16);
		MultiByteToWideChar(CP_UTF8, NULL, DBUser, 64, _DBUser, 64);
		MultiByteToWideChar(CP_UTF8, NULL, DBPassword, 64, _DBPassword, 64);
		MultiByteToWideChar(CP_UTF8, NULL, DBName, 64, _DBName, 64);
		_configParser.CloseFile();
	}

	DBConnectorTLS::DBConnectorTLS(WCHAR* DBIP, WCHAR* User, WCHAR* Password, WCHAR* DBName, int DBPort)
	{
		_tlsIndex = TlsAlloc();
		StringCchCopy(_DBIP, 16, DBIP);
		StringCchCopy(_DBUser, 64, User);
		StringCchCopy(_DBPassword, 64, Password);
		StringCchCopy(_DBName, 64, DBName);
		_DBPort = DBPort;

		_arrayIndex = 0;
	}

	DBConnectorTLS::~DBConnectorTLS()
	{
		TlsFree(_tlsIndex);

		for (int i = 0; i < _arrayIndex; i++)
		{
			delete _DBConnectorPointerArray[i];
		}
	}

	bool DBConnectorTLS::Connect(void)
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return false;
			}
		}

		return connector->Connect();
	}

	bool DBConnectorTLS::Disconnect(void)
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return false;
			}
		}

		connector->Disconnect();
		return true;
	}

	bool DBConnectorTLS::Query(const WCHAR* szStringFormat, ...)
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
				bool ret = connector->Connect();
				if (!ret)
					cd.Crash();
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return false;
			}
		}

		va_list args;
		va_start(args, szStringFormat);
		bool ret = connector->Query(szStringFormat, args);
		va_end(args);

		connector->_SQLResult = mysql_store_result(connector->_MySQLPointer);

		return ret;
	}

	bool DBConnectorTLS::Query_Save(const WCHAR* szStringFormat, ...)
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
				bool ret = connector->Connect();
				if (!ret)
					cd.Crash();
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return false;
			}
		}

		va_list args;
		va_start(args, szStringFormat);
		connector->Query(szStringFormat, args);
		va_end(args);

		return true;
	}

	MYSQL_ROW DBConnectorTLS::FetchRow()
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return NULL;
			}
		}

		return connector->FetchRow();
	}

	void DBConnectorTLS::FreeResult(void)
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return;
			}
		}

		connector->FreeResult();
	}

	int DBConnectorTLS::GetLastError(void)
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return 0;
			}
		}

		return connector->GetLastError();
	}

	WCHAR* DBConnectorTLS::GetLastErrorMsg(void)
	{
		DBConnector* connector = (DBConnector*)TlsGetValue(_tlsIndex);
		if (connector == nullptr)
		{
			int arrayIndex = InterlockedIncrement((LONG*)&_arrayIndex) - 1;
			if (arrayIndex < DBConnectorArray)
			{
				connector = new DBConnector(_DBIP, _DBUser, _DBPassword, _DBName, _DBPort);
				_DBConnectorPointerArray[arrayIndex] = connector;
				TlsSetValue(_tlsIndex, connector);
			}
			else
			{
				LOG(L"DB", Logger::LogLevel::LOG_LEVEL_ERROR, L"TLS Array Full");
				return nullptr;
			}
		}

		return connector->GetLastErrorMsg();
	}
}