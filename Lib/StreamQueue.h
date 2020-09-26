#ifndef __MINLIB_STREAM_QUEUE__
#define __MINLIB_STREAM_QUEUE__
#pragma once

//////////////////////////////////////////////////////////////
// StreamQueue :
// 설명 : 직렬화 원형 큐 클래스
//////////////////////////////////////////////////////////////
namespace MinLib
{
	class StreamQueue
	{
		char* _bufferPointer;	// 버퍼 시작 주소 포인터
		int _front;				// 읽기 시작점
		int _rear;				// 쓰기 시작점
		int _size;				// 버퍼 크기
		int _availableSize;		// 저장 가능한 버퍼 크기
		int _useSize;			// 저장된 데이터 크기
	protected:
		void Initial(int iBufferSize);
	public:
		StreamQueue(int iBufferSize = 10000);
		~StreamQueue();
		// 버퍼 해제후 size만큼 재할당
		inline void Resize(int size);
		// 저장 가능한 버퍼 크기 리턴
		inline int	GetBufferSize();
		// 저장중인 데이터 크기 리턴
		inline int	GetUseSize();
		// 더 저장가능한 데이터 크기 리턴
		inline int	GetFreeSize();
		// 한번에 읽을수 있는 데이터 크기 리턴
		inline int	GetNotBrokenGetSize();
		// 한번에 저장 가능한 데이터 크기 리턴
		inline int	GetNotBrokenPutSize();
		// 데이터 저장
		int	Enqueue(char* chpData, int iSize);
		// 데이터 빼기
		int	Dequeue(char* chpDest, int iSize);
		// 데이터 빼지않고 읽기
		int	Peek(char* chpDest, int iSize, int jumpSize = 0);
		// 데이터 앞부분 삭제
		inline void RemoveData(int iSize);
		// 데이터 뒷부분 추가
		inline void MoveWritePos(int iSize);
		// 데이터 모두 삭제
		inline void ClearBuffer();
		// 버퍼 시작 주소 포인터 리턴
		inline char* GetBufferPtr();
		// 읽기 시작 주소 포인터 리턴
		inline char* GetReadBufferPtr();
		// 쓰기 시작 주소 포인터 리턴
		inline char* GetWriteBufferPtr();
		// 정리
		inline void Clear();
	};

	/*----------------------------------------------------------*/
	// StreamQueue::Resize (public)
	// 설명 : 버퍼의 전체 크기 변경(재할당)
	// 인자 : (int) 바이트 수
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	inline void StreamQueue::Resize(int size)
	{
		delete[] _bufferPointer;
		_bufferPointer = new char[size];
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetBufferSize (public)
	// 설명 : 버퍼의 전체 크기 얻기
	// 인자 : 
	// 리턴 : (int) 바이트 수
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetBufferSize()
	{
		return _availableSize;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetUseSize (public)
	// 설명 : 현재 사용중인 바이트 수 얻기.
	// 인자 : 
	// 리턴 : (int) 바이트 수
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetUseSize()
	{
		//return _useSize;
		// 스레드에 안전하도록 지역변수에 저장
		int front = _front;
		int rear = _rear;
		if (front < rear)
			return rear - front;
		else if (rear < front)
			return rear + (_size - front);
		// front = rear 
		else
			return 0;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetFreeSize (public)
	// 설명 : 현재 버퍼에 남은 바이트 수 얻기.
	// 인자 : 
	// 리턴 : (int) 바이트 수.
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetFreeSize()
	{
		//return _availableSize - _useSize;
		// 스레드에 안전하도록 지역변수에 저장
		int front = _front;
		int rear = _rear;
		if (front < rear)
			return _availableSize - (rear - front);
		else if (rear < front)
			return front - rear - 1;
		// front = rear 
		else
			return _availableSize;
		//return _availableSize - _useSize;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetNotBrokenGetSize (public)
	// 설명 : 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
	// 인자 : 
	// 리턴 : (int) 바이트 수
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetNotBrokenGetSize()
	{
		// Enqueue Dequeue를 동시에 안전하게 하기위해 정확도를 버린다
		//if (_front == _availableSize)
		//	return _availableSize;
		//else
		//	return _availableSize - _front;

		// 스레드에 안전하도록 지역변수에 저장
		int front = _front;
		int rear = _rear;
		if (front < rear)
			return rear - front;
		else if (rear < front)
		{
			if (front == _availableSize)
				return rear + 1;
			else
				return _availableSize - front;
		}
		else
			return 0;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetNotBrokenPutSize (public)
	// 설명 : 한번에 저장할 수 있는 크기를 얻음
	// 인자 : 
	// 리턴 : (int) 바이트 수
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetNotBrokenPutSize()
	{
		// 스레드에 안전하도록 지역변수에 저장
		int front = _front;
		int rear = _rear;
		if (front < rear)
		{
			if (rear == _availableSize)
				return  front;
			else
				return _availableSize - rear;
		}
		else if (rear < front)
			return front - rear - 1;
		else
		{
			if (rear == _availableSize)
				return _availableSize;
			else
				return _availableSize - rear;
		}
	}
	/*----------------------------------------------------------*/
	// StreamQueue::RemoveData (public)
	// 설명 : 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	// 인자 : (int)크기
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	inline void StreamQueue::RemoveData(int iSize)
	{
		int front = _front;
		front += iSize;
		if (_availableSize < front)
			front %= _size;
		_front = front;
		//InterlockedAdd((LONG *)&_useSize, -iSize);
		//_useSize -= iSize;
	}
	inline void StreamQueue::MoveWritePos(int iSize)
	{
		int rear = _rear;
		rear += iSize;
		if (_availableSize < rear)
			rear %= _size;
		_rear = rear;
		//InterlockedAdd((LONG *)&_useSize, iSize);
		//_useSize += iSize;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::ClearBuffer (public)
	// 설명 : 버퍼의 모든 데이터 삭제
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	inline void StreamQueue::ClearBuffer()
	{
		_rear = _front;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetBufferPtr (public)
	// 설명 : 버퍼의 현재 포인터 얻음
	// 인자 : 
	// 리턴 :(char *) 버퍼 포인터
	/*----------------*////////////////////////*----------------*/
	inline char* StreamQueue::GetBufferPtr()
	{
		return _bufferPointer;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetReadBufferPtr (public)
	// 설명 : 버퍼의 ReadPos 포인터 얻음
	// 인자 : 
	// 리턴 : (char *) 버퍼 포인터
	/*----------------*////////////////////////*----------------*/
	inline char* StreamQueue::GetReadBufferPtr()
	{
		if (_front == _availableSize)
			return _bufferPointer;
		else
			return _bufferPointer + _front + 1;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetWriteBufferPtr (public)
	// 설명 : 버퍼의 WritePos 포인터 리턴
	// 인자 : 
	// 리턴 : (char *) 버퍼 포인터
	/*----------------*////////////////////////*----------------*/
	inline char* StreamQueue::GetWriteBufferPtr()
	{
		if (_rear == _availableSize)
			return _bufferPointer;
		else
			return _bufferPointer + _rear + 1;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::Clear (public)
	// 설명 : front, rear을 0으로 되돌리며 버퍼 초기화
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	inline void StreamQueue::Clear()
	{
		_front = 0;
		_rear = 0;
	}
}
#endif // !__MINLIB_STREAM_QUEUE__