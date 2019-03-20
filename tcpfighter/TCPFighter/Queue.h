#pragma once
#define	QUEUESIZE 100

template <typename Q>
class Queue
{
	Q _queue[QUEUESIZE];
	int _front, _rear;
	int _size;
	
public:
	Queue() 
	{
		_front = -1; //Ç×»ó ºó°÷ °¡¸®Å´
		_rear = -1;
		_size = 0;
	}
	bool EnQueue(Q item) 
	{
		if (isFull())
			return false;
		_rear = (_rear + 1) % QUEUESIZE;
		_queue[_rear] = item;
		++_size;
		return true;
	}
	bool DeQueue(Q * item)
	{
		if (isEmpty())
			return false;
		_front = (_front + 1) % QUEUESIZE;
		*item = _queue[_front];
		--_size;
		return true;
	}
	bool DeQueue()
	{
		if (isEmpty())
			return false;
		_front = (_front + 1) % QUEUESIZE;
		--_size;
		return true;
	}
	bool isFull()
	{
		return _size == QUEUESIZE;
	}
	bool isEmpty() 
	{
		return _size == 0;
	}

	bool PeekQueue(int pos, Q * item)
	{
		if (_size < pos)
			return false;
		*item = _queue[(_front + 1 + pos) % QUEUESIZE];
		return true;
	}

	int GetSize()
	{
		return _size;
	}
};
