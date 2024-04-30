#pragma once

template <typename T>
class CList
{
public:
	struct Node
	{
		T		_data;
		Node *	_prev;
		Node *	_next;
	};

	class iterator
	{
	protected:
	public:
		Node * _node;
		//인자로 들어온 Node 포인터를 저장
		iterator(Node *node = nullptr);
		iterator(const iterator& copy);
		//현재 노드를 다음 노드로 이동
		iterator operator++(int);
		iterator operator++();
		bool operator==(iterator iter);
		bool operator!=(iterator iter);
		//현재 노드의 데이터를 뽑음
		T& operator*();
		friend class CList;
	};
public:
	CList();
	~CList();

	//첫번째 노드를 가리키는 이터레이터 리턴
	iterator begin();
	//Tail 노드를 가리키는(데이터가 없는 진짜 끝 노드) 이터레이터를 리턴
	//	또는 끝으로 인지할 수 있는 이터레이터를 리턴
	iterator end();
	void push_front(T data);
	void push_back(T data);
	void clear();
	int size() { return _size; };
	bool is_empty();

	//- 이터레이터의 그 노드를 지움.
	//- 그리고 지운 노드의 다음 노드를 카리키는 이터레이터 리턴
	iterator erase(iterator itor);
private:
	int _size = 0;
	Node _head;
	Node _tail;

};

template<typename T>
inline CList<T>::CList()
{
	_size = 0;
	_head._next = &_tail;
	_head._prev = nullptr;
	_tail._next = nullptr;
	_tail._prev = &_head;
}

template<typename T>
inline CList<T>::~CList()
{
	clear();
}

template<typename T>
inline typename CList<T>::iterator CList<T>::begin()
{
	iterator iter(_head._next);
	return iter;
}

template<typename T>
inline typename CList<T>::iterator CList<T>::end()
{
	iterator iter(&_tail);
	return iter;
}

template<typename T>
inline void CList<T>::push_front(T data)
{
	Node * newNode = new Node;
	newNode->_data = data;
	newNode->_next = _head._next;
	newNode->_prev = &_head;
	_head._next->_prev = newNode;
	_head._next = newNode;
	++_size;
}

template<typename T>
inline void CList<T>::push_back(T data)
{
	Node * newNode  = new Node;
	newNode->_data = data;
	newNode->_next = &_tail;
	newNode->_prev = _tail._prev;
	_tail._prev->_next = newNode;
	_tail._prev = newNode;
	++_size;
}

template<typename T>
inline void CList<T>::clear()
{
	iterator iter(_head._next);
	for (int i = 0; i < _size; ++i)
		iter = erase(iter);
	
}

template<typename T>
inline bool CList<T>::is_empty()
{
	if (!size())
		return true;
	return false;
}
template<typename T>
inline typename CList<T>::iterator CList<T>::erase(iterator itor)
{
	itor._node->_next->_prev = itor._node->_prev;
	itor._node->_prev->_next = itor._node->_next;
	delete itor++._node;
	--_size;
	return itor;
}

template<typename T>
inline CList<T>::iterator::iterator(Node * node)
{
	this->_node = node;
}

template<typename T>
inline CList<T>::iterator::iterator(const iterator & copy)
{
	this->_node = copy._node;
}

template<typename T>
inline typename CList<T>::iterator CList<T>::iterator::operator++(int)
{
	iterator copy(*this);
	_node = _node->_next;
	return copy;
}

template<typename T>
inline typename CList<T>::iterator CList<T>::iterator::operator++()
{
	_node = _node->_next;
	return _node;
}

template<typename T>
inline bool CList<T>::iterator::operator==(iterator iter)
{
	if (this->_node == iter._node)
		return true;
	return false;
}

template<typename T>
inline bool CList<T>::iterator::operator!=(iterator iter)
{
	return !operator==(iter);
}

template<typename T>
inline T & CList<T>::iterator::operator*()
{
	return this->_node->_data;
}
