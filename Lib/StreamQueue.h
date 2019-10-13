#ifndef __MINLIB_STREAM_QUEUE__
#define __MINLIB_STREAM_QUEUE__
#pragma once
//스트림 큐(링버퍼)
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
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	/////////////////////////////////////////////////////////////////////////
	// 크기 변경.
	// Parameters: (int)새로운 크기.
	// Return:
	/////////////////////////////////////////////////////////////////////////
	inline void StreamQueue::Resize(int size)
	{
		delete[] _bufferPointer;
		_bufferPointer = new char[size];
	}
	/*----------------------------------------------------------*/
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 크기 얻기
	// Parameters:
	// Return: (int)버퍼의 크기.
	/////////////////////////////////////////////////////////////////////////
	inline int StreamQueue::GetBufferSize()
	{
		return _availableSize;
	}
	/*----------------------------------------------------------*/
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	/////////////////////////////////////////////////////////////////////////
	// 현재 사용중인 용량 얻기.
	// Parameters: 없음.
	// Return: (int)사용중인 용량.
	/////////////////////////////////////////////////////////////////////////
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
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	/////////////////////////////////////////////////////////////////////////
	// 현재 버퍼에 남은 용량 얻기.
	// Parameters: 없음.
	// Return: (int)남은용량.
	/////////////////////////////////////////////////////////////////////////
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
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	/////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
	// (끊기지 않은 길이)
	// Parameters: 없음.
	// Return: (int)사용가능 용량.
	////////////////////////////////////////////////////////////////////////
	inline int StreamQueue::GetNotBrokenGetSize()
	{
		// Enqueue Dequeue를 동시에 안전하게 하기위해 정확도를 버린다
		//if (_front == _availableSize)
		//	return _availableSize;
		//else
		//	return _availableSize - _front;

		// 더 정확하지만 멀티스레드에 안전하지않음
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
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 
	// 인자 : 
	// 리턴 :
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
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	/////////////////////////////////////////////////////////////////////////
	// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	// Parameters: (int)크기.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
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
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 버퍼의 모든 데이터 삭제
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 모든 데이타 삭제.
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	inline void StreamQueue::ClearBuffer()
	{
		_rear = _front;
	}
	/*----------------------------------------------------------*/
	// Class::Method (virtual) (private) (protected) (public)
	// 설명 : 버퍼의 포인터 얻음
	// 인자 : 
	// 리턴 :(char *) 버퍼 포인터
	/*----------------*////////////////////////*----------------*/
	inline char* StreamQueue::GetBufferPtr()
	{
		return _bufferPointer;
	}
	/*----------------------------------------------------------*/
	// Class::Method (virtual) (private) (protected) (public)
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
	// Class::Method (virtual) (private) (protected) (public)
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