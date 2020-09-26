#ifndef __MINLIB_RANDOM_NUMBER__
#define __MINLIB_RANDOM_NUMBER__
#pragma once
#include <random>
#include <stack>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <strsafe.h>
#include "memoryPool.h"
namespace MinLib
{
	constexpr auto ITEMNAME_LEN = 20;

	struct RANDOM_ITEM
	{
		int minValue;
		int maxValue;
		WCHAR itemName[ITEMNAME_LEN];
	};

	class Random
	{
		std::random_device			_random_device;
		std::mt19937 				_m_19937;

		std::vector<int>			_ratioVector;
		std::vector<int>			_usedVector;
		std::vector<RANDOM_ITEM*>	_itemInfo;
		MemoryPoolTLS<RANDOM_ITEM>	_itemPool;

		int							_totalRatio = 0;
		bool						_reUse = false;
		SRWLOCK						_lock;

	public:
		Random();
		~Random();

		void ReInitialize();
		void AddItem(int ratio, const WCHAR* name);
		void Shaffle();
		bool GetNumber(int* outNumber, WCHAR** name);
	};
}
#endif // !__MINLIB_RANDOM_NUMBER__