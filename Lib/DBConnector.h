#ifndef __MINLIB_DB_CONNECTOR__
#define __MINLIB_DB_CONNECTOR__
#pragma once
#pragma comment(lib,"thirdparty/mysql-c/lib/vs14/mysqlclient.lib")

#include <Windows.h>
#include <strsafe.h>
#include "Logger.h"
#include "Parser.h"
#include "CrashDump.h"
#include "thirdparty/mysql-c/include/mysql.h"
#include "thirdparty/mysql-c/include/errmsg.h"
namespace MinLib
{
	class DBConnector
	{
	public:
		enum en_DB_CONNECTOR
		{
			eQUERY_MAX_LEN = 2048
		};

	private:
		//-------------------------------------------------------------
		// MySQL 연결객체 본체
		//-------------------------------------------------------------
		MYSQL		_MySQL;

		//-------------------------------------------------------------
		// MySQL 연결객체 포인터. 위 변수의 포인터임. 
		// 이 포인터의 null 여부로 연결상태 확인.
		//-------------------------------------------------------------
		MYSQL* _MySQLPointer = { nullptr };

		//-------------------------------------------------------------
		// 쿼리를 날린 뒤 Result 저장소.
		//
		//-------------------------------------------------------------
		MYSQL_RES* _SQLResult;

		WCHAR		_DBIP[16] = { 0, };
		WCHAR		_DBUser[64] = { 0, };
		WCHAR		_DBPassword[64] = { 0, };
		WCHAR		_DBName[64] = { 0, };
		int			_DBPort = { 0 };


		WCHAR		_Query[eQUERY_MAX_LEN] = { 0, };
		char		_QueryUTF8[eQUERY_MAX_LEN] = { 0, };

		int			_LastError = { 0 };
		WCHAR		_LastErrorMsg[128] = { 0, };

		//////////////////////////////////////////////////////////////////////
		// mysql 의 LastError 를 맴버변수로 저장한다.
		//////////////////////////////////////////////////////////////////////
		void		SaveLastError();
	protected:
		Parser		_configParser;

	public:
		DBConnector(const char* config);
		DBConnector(WCHAR* DBIP, WCHAR* User, WCHAR* Password, WCHAR* DBName, int DBPort);
		virtual		~DBConnector();

		//////////////////////////////////////////////////////////////////////
		// MySQL DB 연결
		//////////////////////////////////////////////////////////////////////
		bool		Connect(void);

		//////////////////////////////////////////////////////////////////////
		// MySQL DB 끊기
		//////////////////////////////////////////////////////////////////////
		bool		Disconnect(void);


		//////////////////////////////////////////////////////////////////////
		// 쿼리 날리고 결과셋 임시 보관
		//
		//////////////////////////////////////////////////////////////////////
		bool		Query(const WCHAR* StringFormat, va_list args);
		bool		Query(const WCHAR* StringFormat, ...);
		bool		Query_Save(const WCHAR* szStringFormat, ...);	// DBWriter 스레드의 Save 쿼리 전용
																// 결과셋을 저장하지 않음.

		//////////////////////////////////////////////////////////////////////
		// 쿼리를 날린 뒤에 결과 뽑아오기.
		//
		// 결과가 없다면 NULL 리턴.
		//////////////////////////////////////////////////////////////////////
		MYSQL_ROW	FetchRow(void);

		//////////////////////////////////////////////////////////////////////
		// 한 쿼리에 대한 결과 모두 사용 후 정리.
		//////////////////////////////////////////////////////////////////////
		void		FreeResult(void);


		//////////////////////////////////////////////////////////////////////
		// Error 얻기.한 쿼리에 대한 결과 모두 사용 후 정리.
		//////////////////////////////////////////////////////////////////////
		int			GetLastError(void) { return _LastError; };
		WCHAR* GetLastErrorMsg(void) { return _LastErrorMsg; }

		friend class DBConnectorTLS;
	};


	// TLS ver
	constexpr int DBConnectorArray = 20;
	class DBConnectorTLS
	{
		DWORD		_tlsIndex = { 0 };
		WCHAR		_DBIP[16] = { 0, };
		WCHAR		_DBUser[64] = { 0, };
		WCHAR		_DBPassword[64] = { 0, };
		WCHAR		_DBName[64] = { 0, };
		int			_DBPort = { 0 };

		int			_arrayIndex = { 0 };
		DBConnector* _DBConnectorPointerArray[DBConnectorArray] = { nullptr, };
		Parser		_configParser;

		void		SaveLastError();

	public:
		DBConnectorTLS(const char* config);
		DBConnectorTLS(WCHAR* DBIP, WCHAR* User, WCHAR* Password, WCHAR* DBName, int DBPort);
		~DBConnectorTLS();

		bool		Connect(void);

		bool		Disconnect(void);

		bool		Query(const WCHAR* szStringFormat, ...);
		bool		Query_Save(const WCHAR* szStringFormat, ...);

		MYSQL_ROW	FetchRow(void);

		void		FreeResult(void);

		int			GetLastError(void);
		WCHAR* GetLastErrorMsg(void);
	};
}
#endif // !__MINLIB_DB_CONNECTOR__