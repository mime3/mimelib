#ifndef __MINLIB_STREAM_QUEUE__
#define __MINLIB_STREAM_QUEUE__
#pragma once

//////////////////////////////////////////////////////////////
// StreamQueue :
// ���� : ����ȭ ���� ť Ŭ����
//////////////////////////////////////////////////////////////
namespace MinLib
{
	class StreamQueue
	{
		char* _bufferPointer;	// ���� ���� �ּ� ������
		int _front;				// �б� ������
		int _rear;				// ���� ������
		int _size;				// ���� ũ��
		int _availableSize;		// ���� ������ ���� ũ��
		int _useSize;			// ����� ������ ũ��
	protected:
		void Initial(int iBufferSize);
	public:
		StreamQueue(int iBufferSize = 10000);
		~StreamQueue();
		// ���� ������ size��ŭ ���Ҵ�
		inline void Resize(int size);
		// ���� ������ ���� ũ�� ����
		inline int	GetBufferSize();
		// �������� ������ ũ�� ����
		inline int	GetUseSize();
		// �� ���尡���� ������ ũ�� ����
		inline int	GetFreeSize();
		// �ѹ��� ������ �ִ� ������ ũ�� ����
		inline int	GetNotBrokenGetSize();
		// �ѹ��� ���� ������ ������ ũ�� ����
		inline int	GetNotBrokenPutSize();
		// ������ ����
		int	Enqueue(char* chpData, int iSize);
		// ������ ����
		int	Dequeue(char* chpDest, int iSize);
		// ������ �����ʰ� �б�
		int	Peek(char* chpDest, int iSize, int jumpSize = 0);
		// ������ �պκ� ����
		inline void RemoveData(int iSize);
		// ������ �޺κ� �߰�
		inline void MoveWritePos(int iSize);
		// ������ ��� ����
		inline void ClearBuffer();
		// ���� ���� �ּ� ������ ����
		inline char* GetBufferPtr();
		// �б� ���� �ּ� ������ ����
		inline char* GetReadBufferPtr();
		// ���� ���� �ּ� ������ ����
		inline char* GetWriteBufferPtr();
		// ����
		inline void Clear();
	};

	/*----------------------------------------------------------*/
	// StreamQueue::Resize (public)
	// ���� : ������ ��ü ũ�� ����(���Ҵ�)
	// ���� : (int) ����Ʈ ��
	// ���� :
	/*----------------*////////////////////////*----------------*/
	inline void StreamQueue::Resize(int size)
	{
		delete[] _bufferPointer;
		_bufferPointer = new char[size];
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetBufferSize (public)
	// ���� : ������ ��ü ũ�� ���
	// ���� : 
	// ���� : (int) ����Ʈ ��
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetBufferSize()
	{
		return _availableSize;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetUseSize (public)
	// ���� : ���� ������� ����Ʈ �� ���.
	// ���� : 
	// ���� : (int) ����Ʈ ��
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetUseSize()
	{
		//return _useSize;
		// �����忡 �����ϵ��� ���������� ����
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
	// ���� : ���� ���ۿ� ���� ����Ʈ �� ���.
	// ���� : 
	// ���� : (int) ����Ʈ ��.
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetFreeSize()
	{
		//return _availableSize - _useSize;
		// �����忡 �����ϵ��� ���������� ����
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
	// ���� : ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
	// ���� : 
	// ���� : (int) ����Ʈ ��
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetNotBrokenGetSize()
	{
		// Enqueue Dequeue�� ���ÿ� �����ϰ� �ϱ����� ��Ȯ���� ������
		//if (_front == _availableSize)
		//	return _availableSize;
		//else
		//	return _availableSize - _front;

		// �����忡 �����ϵ��� ���������� ����
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
	// ���� : �ѹ��� ������ �� �ִ� ũ�⸦ ����
	// ���� : 
	// ���� : (int) ����Ʈ ��
	/*----------------*////////////////////////*----------------*/
	inline int StreamQueue::GetNotBrokenPutSize()
	{
		// �����忡 �����ϵ��� ���������� ����
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
	// ���� : ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	// ���� : (int)ũ��
	// ���� :
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
	// ���� : ������ ��� ������ ����
	// ���� : 
	// ���� :
	/*----------------*////////////////////////*----------------*/
	inline void StreamQueue::ClearBuffer()
	{
		_rear = _front;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetBufferPtr (public)
	// ���� : ������ ���� ������ ����
	// ���� : 
	// ���� :(char *) ���� ������
	/*----------------*////////////////////////*----------------*/
	inline char* StreamQueue::GetBufferPtr()
	{
		return _bufferPointer;
	}
	/*----------------------------------------------------------*/
	// StreamQueue::GetReadBufferPtr (public)
	// ���� : ������ ReadPos ������ ����
	// ���� : 
	// ���� : (char *) ���� ������
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
	// ���� : ������ WritePos ������ ����
	// ���� : 
	// ���� : (char *) ���� ������
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
	// ���� : front, rear�� 0���� �ǵ����� ���� �ʱ�ȭ
	// ���� : 
	// ���� :
	/*----------------*////////////////////////*----------------*/
	inline void StreamQueue::Clear()
	{
		_front = 0;
		_rear = 0;
	}
}
#endif // !__MINLIB_STREAM_QUEUE__