#include "pch.h"
#include "Parser.h"

Parser::Parser()
{
	for (auto & box : _box)
	{
		box = 0;
	}
	_blockCount = 0;
	_fileSize = 0;
	_flag = 0;
}

void Parser::PrintAll()
{
	char a[3]; 
	a[0] = *_curPointer;
	a[1] = '2';
	a[2] = '\0';
	printf("%s",a);
}

bool Parser::OpenFile(const char * fileName)
{
	if (fopen_s(&_file, fileName, "rb") != 0)
		return false;

	int fd = _fileno(_file);
	_fileSize = _filelength(fd);
	_filePointer = (char *)malloc(_fileSize);
	fread_s(_filePointer, _fileSize, _fileSize, 1, _file);
	_curPointer = _filePointer;
	_fileEndPointer = _filePointer + _fileSize;
	
	_checkChar[0] = _filePointer;
	_checkChar[1] = _filePointer + 1; 
	//설마 두글자도 안되냐?
	if (_checkChar[1] == nullptr)
		return false;
	UpdateFlag();
	CheckBlock();
	fclose(_file);
	return true;
}

void Parser::CheckBlock()
{
	Block block;
	char buffer[2];
	while (_checkChar[1] != _fileEndPointer)
	{
		memset(buffer, 0, 2);
		int bufPos = 0;
		if (*_checkChar[0] == ':')
		{
			buffer[bufPos] = ':';
			bufPos = (bufPos + 1) % 2;
		}
	
		while (_checkChar[1] != _fileEndPointer)
		{
			if (buffer[0] == ':' && buffer[1] == ':')
				break;
	
			Read();
			UpdateFlag();
			if (IsComment())
				continue;
	
			if (*_checkChar[0] == ':')
			{
				buffer[bufPos] = ':';
				bufPos = (bufPos + 1) % 2;
				continue;
			}
		}
		// 현재 *_checkChar이 [:][?] 상태

		if (!GetNextWord(block._name,_fileEndPointer))
			return;
		// 현재 *_checkChar이 [x][] 상태
		while (_checkChar[1] != _fileEndPointer)
		{
			if (*_checkChar[0] == '{')
				break;
			Jump();
		}
		if (_checkChar[1] == _fileEndPointer)
			return;
		block._startPoint = _checkChar[0];
	
		while (_checkChar[1] != _fileEndPointer)
		{
			if (*_checkChar[0] == '}')
				break;
			Jump();
		}
		if (*_checkChar[0] != '}' && _checkChar[1] == _fileEndPointer )
			return;
		block._endPoint = _checkChar[0];
		_block[_blockCount++] = block;
	}
}

bool Parser::CloseFile()
{
	free(_filePointer);
	for (auto & box : _box)
	{
		box = 0;
	}
	_blockCount = 0;
	_fileSize = 0;
	_flag = 0;
	return true;
}

void Parser::Read()
{
	_checkChar[0]++;
	_checkChar[1]++;
	/*
	_box[0] = _box[1];
	_box[1] = *_curPointer++;
	*/
}

void Parser::UpdateFlag()
{
	//플래그 먼저 확인
	// 주석 모드일경우
	if (GetFlag() & (BLOCKCOMMENT | LINECOMMENT))
	{
		// 주석해제
		if (*_checkChar[0] == CARRIAGE_RETURN && *_checkChar[1] == LINE_FEED)
			OffFlag(LINECOMMENT);
		if (*_checkChar[0] == '*' && *_checkChar[1] == '/')
		{
			Read();
			Read();
			UpdateFlag();
			OffFlag(BLOCKCOMMENT);
		}
	}
	// 주석모드가 아닐때
	else
	{
		// 주석으로 
		if (*_checkChar[0] == '/' && *_checkChar[1] == '/')
			SetFlag(LINECOMMENT);

		if (*_checkChar[0] == '/' && *_checkChar[1] == '*')
		{
			SetFlag(BLOCKCOMMENT);
			Read();
			Read();
		}

		// 문자열 모드일때 
		if (GetFlag() & STRINGMODE)
		{
			// 특수처리모드일때
			if (GetFlag() & SPECIALMODE)
			{
				Read();
				OffFlag(SPECIALMODE); //해제
			}
			// 아닐때 
			else
			{
				if (*_checkChar[0] == '\\')
					SetFlag(SPECIALMODE);
				// 문자열모드 종료
				if (*_checkChar[0] == '"')
					SetFlag(STRINGMODE);
			}
		}
		// 문자열을 만남
		else
		{
			if (*_checkChar[0] == '"')
			{
				SetFlag(STRINGMODE);
				Read();
			}
		}
	}
}

bool Parser::GetEqual()
{
	while (_checkChar[1] != _fileEndPointer)
	{
		if (*_checkChar[0] == '=')
			return true;
		Read();
		UpdateFlag();
		if (IsComment())
			continue;
		if (*_checkChar[0] == '=')
			return true;
	}
	return false;
}
// 단어의 경계체크
bool Parser::CheckBlank()
{
	if (*_checkChar[0] == ',')
		return true;
	if (*_checkChar[0] == ' ')
		return true;
	if (*_checkChar[0] == SPACE) // 스페이스
		return true;
	if (*_checkChar[0] == BACK_SPACE) // 백스페이스
		return true;
	if (*_checkChar[0] == TAB) // 탭
		return true;
	if (*_checkChar[0] == LINE_FEED) // 라인피드
		return true;
	if (*_checkChar[0] == CARRIAGE_RETURN) // 캐리지리턴
		return true;
	if (*_checkChar[0] == '=')
		return true;
	
	return false;
}
// 사용전 상태는 [x][?] 임을 가정 , Jump후 상태는 NonComment Mode 에 [y][?]
void Parser::Jump()
{
	while (_checkChar[1] != _fileEndPointer)
	{
		Read();
		UpdateFlag();
		if (IsComment())
			continue;
		if (!CheckBlank())
			break;
	}
}

// 실행후 결과는 [x][] , 찾은단어의 끝부분
bool Parser::GetNextWord(char * buffer, char * blockEndPoint)
{
	if (_checkChar[1] == blockEndPoint)
		return false;
	
	char buf[64];
	int length = 0;
	Jump();
	// Jump()가 현재 *_checkChar[0]이 주석이 아닌글자임을 보장
	buf[length++] = *_checkChar[0]; 
	while (_checkChar[1] != _fileEndPointer)
	{
		Read();
		UpdateFlag();
		if (IsComment())
			continue;
		if (CheckBlank())
			break;
		buf[length++] = *_checkChar[0];
	}
	buf[length] = '\0';
	memcpy(buffer, buf, sizeof(buf));
	return true;
}

bool Parser::GetValue_BYTE(const char * word, BYTE & outBYTE, const char * blockName)
{

	// 블록 시작점, 끝점
	char *blockEndPoint = _fileEndPointer;
	for (auto block : _block)
	{
		if (strcmp(block._name, blockName) == 0)
		{
			_checkChar[0] = block._startPoint;
			_checkChar[1] = _checkChar[0] + 1;
			blockEndPoint = block._endPoint;
			break;
		}
	}
	// 블록이 없을때? 처리가 아직 없다.
	// 일단 블록을 찾았다고 가정
	char buffer[64];
	while (GetNextWord(buffer, blockEndPoint))
	{
		if (strcmp(buffer, word) != 0)
			continue;

		if (!GetEqual())
			return false;

		GetNextWord(buffer, blockEndPoint);
		outBYTE = (BYTE)atoi(buffer);
		break;
	}
	return true;
}

bool Parser::GetValue_Int(const char * word, int & outInt, const char * blockName)
{
	
	// 블록 시작점, 끝점
	char *blockEndPoint = _fileEndPointer;
	for (auto block : _block)
	{
		if (strcmp(block._name, blockName) == 0)
		{
			_checkChar[0] = block._startPoint;
			_checkChar[1] = _checkChar[0] + 1;
			blockEndPoint = block._endPoint;
			break;
		}
	}
	// 블록이 없을때? 처리가 아직 없다.
	// 일단 블록을 찾았다고 가정
	char buffer[64];
	while (GetNextWord(buffer, blockEndPoint))
	{
		if (strcmp(buffer, word) != 0)
			continue;

		if (!GetEqual())
			return false;

		GetNextWord(buffer,blockEndPoint);
		outInt = atoi(buffer);
		break;
	}
	return true;
}

bool Parser::GetValue_Str(const char * word, char * outString, const char * blockName)
{
	char *blockEndPoint = _fileEndPointer;
	for (auto block : _block)
	{
		if (strcmp(block._name, blockName) == 0)
		{
			_checkChar[0] = block._startPoint;
			_checkChar[1] = _checkChar[0] + 1;
			blockEndPoint = block._endPoint;
			break;
		}
	}
	// 블록이 없을때? 처리가 아직 없다.
	// 일단 블록을 찾았다고 가정
	char buffer[64];
	int bufPos = 0;
	while (GetNextWord(buffer, blockEndPoint))
	{
		// 이 단어가 아니다
		if (strcmp(buffer, word) != 0)
			continue;

		// GetEqual 성공하면 [=][?]
		if (!GetEqual())
			return false;

		while (_checkChar[1] != blockEndPoint)
		{
			// 성공하면 checkChar = ["][?]
			if (!IsString())
			{
				Read();
				UpdateFlag();
				continue;
			}
			else
			{
				buffer[bufPos++] = *_checkChar[0];
				Read();
				UpdateFlag();
			}
			if (!IsString())
			{
				buffer[bufPos] = '\0';
				memcpy(outString, buffer, strlen(buffer) + 1);
				return true;
			}
		}
		return false;
	}
	return false;
}

char Parser::GetFlag()
{
	return _flag;
}

void Parser::SetFlag(Flag flag)
{
	_flag ^= flag;
}

void Parser::OffFlag(Flag flag)
{
	_flag &= ~flag;
}

bool Parser::IsComment()
{
	return _flag & (LINECOMMENT | BLOCKCOMMENT);
}

bool Parser::IsString()
{
	return _flag & (STRINGMODE);
}
