#pragma once
struct PacketHeader
{
	BYTE code;	// 패킷코드 0x89 고정.
	BYTE size;	// 패킷 사이즈.
	BYTE type;	// 패킷타입.
	BYTE temp;	// 사용안함.
};

struct Session
{
	SOCKET sock;
	SOCKADDR_IN addr;
	StreamQueue * resvStream;
	StreamQueue * sendStream;
	bool sendFlag;
	bool connected;
	bool headerSaveFlag;
	PacketHeader saveHeader;
	u_int totalRecv;
	u_int totalSend;
	Session();
};

//class StreamBuffer
//{
//	char * _buffer;
//	int _front;
//	int _rear;
//	int _size;
//public: 
//	StreamBuffer(int size);
//	~StreamBuffer();
//	void moveEndIndex();
//	char * GetBuffer();
//	int GetSize();
//	void ReSize(int size);
//	void BlankHeader(int headerSize);
//	void FillHeader(char * buffer, int size);
//	int In(char * buffer, int size);
//	bool In(BYTE buffer);
//	bool In(WORD buffer);
//	bool In(DWORD buffer);
//	int Out(char * buffer, int size);
//	void Out(BYTE * buffer);
//	void Out(WORD * buffer);
//	void Out(DWORD * buffer);
//	template<typename T> StreamBuffer& operator <<(const T& inData);
//	template<typename T> StreamBuffer& operator >>(const T& outData);
//};
//
//template<typename T>
//inline StreamBuffer& StreamBuffer::operator<<(const T & inData)
//{
//	//static_assert(std::is_aritmetic<T>::value || std::is_enum<T>::value , "Wrong Type")
//	In((char*)&inData, sizeof(inData));
//	return (*this);
//}
//
//template<typename T>
//inline StreamBuffer& StreamBuffer::operator>>(const T & outData)
//{
//	//static_assert(std::is_aritmetic<T>::value || std::is_enum<T>::value, "Wrong Type")
//	Out((char*)&outData, sizeof(outData));
//	return (*this);
//}
