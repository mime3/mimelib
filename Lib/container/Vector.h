#pragma once
#ifndef MINLIB_VECTOR
#define MINLIB_VECTOR

#pragma warning (disable : 6378)
#include <stdlib.h>
#include <memory>
namespace MinLib
{
	template <typename T>
	class Vector final
	{
		class iterator
		{
			friend class Vector;
		public:
			iterator(T* data);

		public:
			iterator operator++(int);
			iterator operator++();
			bool operator==(iterator iter);
			bool operator!=(iterator iter);
			T& operator*();

		private:
			T* data_;
		};

	public:
		Vector(int size);
		Vector(Vector& rhs) = delete;
		~Vector() = default;

		Vector operator=(Vector& rhs) = delete;

	public:
		bool push_back(T input);
		bool pop_back();
		bool pop_front();
		T front();
		T back();
		iterator erase(iterator pos);
		bool reserve(int size);
		bool resize(T* startPointer, int size);
		int size();
		bool empty();
		void clear();

		iterator begin();
		iterator end();

	private:
		void initial(int size);
		
	private:
		T* dataPointer_ = { nullptr };
		int size_ = { 0 };
		int reserve_ = { 0 };
	};

	template<typename T>
	inline bool Vector<T>::resize(T* startPointer, int size)
	{
		T* newDataPointer = static_cast<T*>(malloc(size * sizeof(T)));
		memcpy(newDataPointer, startPointer, size * sizeof(T));
		free(dataPointer_);
		dataPointer_ = newDataPointer;
		reserve_ = size;
		return true;
	}

	template<typename T>
	inline int Vector<T>::size()
	{
		return size_;
	}

	template<typename T>
	inline bool Vector<T>::empty()
	{
		return size_ == 0;
	}

	template<typename T>
	inline void Vector<T>::clear()
	{
		size_ = 0;
	}

	template<typename T>
	inline typename Vector<T>::iterator Vector<T>::begin()
	{
		iterator begin(dataPointer_);
		return begin;
	}

	template<typename T>
	inline typename Vector<T>::iterator Vector<T>::end()
	{
		return Vector<T>::iterator(dataPointer_+ size_);
	}

	template<typename T>
	inline void Vector<T>::initial(int size)
	{
		if (dataPointer_ != nullptr)
			free(dataPointer_);

		dataPointer_ = static_cast<T*>(malloc(size * sizeof(T)));
		size_ = 0;
		reserve_ = size;
	}
	template<typename T>
	inline Vector<T>::Vector(int size)
	{
		initial(size);
	}
	template<typename T>
	inline bool Vector<T>::push_back(T input)
	{
		if (reserve_ == size_)
			resize(dataPointer_, size_ * 2);
		T* savePointer = dataPointer_ + size_;
		*savePointer = input;
		size_++;
		return true;
	}
	template<typename T>
	inline bool Vector<T>::pop_back()
	{
		if (empty())
			return false;
#ifdef _DEBUG
		T* savePointer = dataPointer_ + (size_ - 1);
		memset(savePointer, 0, sizeof(T));
#endif
		size_--;
		return true;
	}
	template<typename T>
	inline bool Vector<T>::pop_front()
	{
		if (empty())
			return false;
		return resize(dataPointer_ + 1, size_ - 1);
	}
	template<typename T>
	inline T Vector<T>::front()
	{
		return *dataPointer_;
	}
	template<typename T>
	inline T Vector<T>::back()
	{
		return *(dataPointer_ + (size_ - 1));
	}
	template<typename T>
	inline bool Vector<T>::reserve(int size)
	{
		return false;
	}

	template<typename T>
	inline Vector<T>::iterator::iterator(T* data)
	{
		data_ = data;
	}

	template<typename T>
	inline typename Vector<T>::iterator Vector<T>::iterator::operator++(int)
	{
		iterator copy(*this);
		data_ += 1;
		return copy;
	}
	template<typename T>
	inline typename Vector<T>::iterator Vector<T>::iterator::operator++()
	{
		data_++;
		return *this;
	}
	template<typename T>
	inline bool Vector<T>::iterator::operator==(iterator iter)
	{
		return data_ == iter.data_;
	}
	template<typename T>
	inline bool Vector<T>::iterator::operator!=(iterator iter)
	{
		return !operator==(iter);
	}
	template<typename T>
	inline T& Vector<T>::iterator::operator*()
	{
		return *data_;
	}
}	
#pragma warning (default : 6378)
#endif // !MINLIB_VECTOR