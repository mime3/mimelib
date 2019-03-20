#include "stdafx.h"
#include "NetWorkObject.h"

Session::Session()
{
	sendFlag = false;
	connected = false;
	headerSaveFlag = false;
	totalRecv = 0;
	totalSend = 0;
	resvStream = new StreamQueue(2000);
	sendStream = new StreamQueue(2000);
}

//StreamBuffer::StreamBuffer(int size)
//{
//	_buffer = new char[size];
//	_front = 0;
//	_rear = 0;
//	_size = size;
//}
//
//StreamBuffer::~StreamBuffer()
//{
//	delete[] _buffer;
//}
//
//void StreamBuffer::moveEndIndex()
//{
//	_rear = _size - 1;
//}
//
//char * StreamBuffer::GetBuffer()
//{
//	return _buffer;
//}
//
//int StreamBuffer::GetSize()
//{
//	return _size;
//}
//
//void StreamBuffer::ReSize(int size)
//{
//	delete[] _buffer;
//	_buffer = new char[size];
//}
//
//void StreamBuffer::BlankHeader(int headerSize)
//{
//	if (_size < headerSize)
//		throw;
//	_rear += headerSize;
//}
//
//void StreamBuffer::FillHeader(char * buffer, int size)
//{
//	if (_size < size)
//		throw;
//	memcpy_s(_buffer, size, buffer, size);
//}
//
//int StreamBuffer::In(char * buffer, int size)
//{
//	if (_size < _rear + size)
//		throw;
//		//size = _size - _rear;
//	memcpy_s(_buffer + _rear, size, buffer, size);
//	_rear += size;
//	return size;
//}
//
//bool StreamBuffer::In(BYTE buffer)
//{
//	if (_size < _rear + sizeof(BYTE))
//		return false;
//	*(BYTE*)(_buffer + _rear) = buffer;
//	_rear += sizeof(BYTE);
//	return true;
//}
//
//bool StreamBuffer::In(WORD buffer)
//{
//	if (_size < _rear + sizeof(WORD))
//		return false;
//	 *(WORD*)(_buffer + _rear) = buffer;
//	 _rear += sizeof(WORD);
//	return true;
//}
//
//bool StreamBuffer::In(DWORD buffer)
//{
//	if (_size < _rear + sizeof(DWORD))
//		return false;
//	*(DWORD*)(_buffer + _rear) = buffer;
//	_rear += sizeof(DWORD);
//	return true;
//}
//
//int StreamBuffer::Out(char * buffer, int size)
//{
//	if (_size < _front + size)
//		throw;
//		//size = _size - _front;
//	memcpy_s(buffer, size, _buffer + _front, size);
//	_front += size;
//	return size;
//}
//
//void StreamBuffer::Out(BYTE * buffer)
//{
//	*buffer = *(BYTE*)(_buffer + _front);
//	_front += sizeof(BYTE);
//}
//
//void StreamBuffer::Out(WORD * buffer)
//{
//	*buffer = *(WORD*)(_buffer + _front);
//	_front += sizeof(WORD);
//}
//
//void StreamBuffer::Out(DWORD * buffer)
//{
//	*buffer = *(DWORD*)(_buffer + _front);
//	_front += sizeof(DWORD);
//}