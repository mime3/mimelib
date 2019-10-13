#include "RandomNumber.h"



Random::Random() : _m_19937(_random_device())
{
	//_m_19937 = new std::mt19937(_random_device());
	InitializeSRWLock(&_lock);
}


Random::~Random()
{
	//delete _m_19937;
}

void Random::ReInit()
{
	AcquireSRWLockExclusive(&_lock);
	if (!_reUse)
	{
		_ratioVector.insert(_ratioVector.end(), _usedVector.begin(), _usedVector.end());
		_usedVector.clear();
	}
	std::shuffle(_ratioVector.begin(), _ratioVector.end(), _m_19937);
	ReleaseSRWLockExclusive(&_lock);
}

void Random::AddItem(int ratio, const WCHAR* name)
{
	AcquireSRWLockExclusive(&_lock);
	RANDOM_ITEM * item = _itemPool.Alloc();
	item->minValue = _totalRatio + 1;
	item->maxValue = _totalRatio + ratio;
	StringCchCopy(item->itemName, ITEMNAME_LEN, name);
	_itemInfo.push_back(item);

	_ratioVector.reserve(_ratioVector.size() + ratio);
	for (int i = _totalRatio + 1; i <= _totalRatio + ratio; i++)
	{
		_ratioVector.push_back(i);
	}
	_totalRatio += ratio;
	ReleaseSRWLockExclusive(&_lock);
}

void Random::Shaffle()
{
	AcquireSRWLockExclusive(&_lock);
	std::shuffle(_ratioVector.begin(), _ratioVector.end(), _m_19937);
	_usedVector.clear();
	_usedVector.reserve(_ratioVector.size());
	ReleaseSRWLockExclusive(&_lock);
}

bool Random::GetNum(int* outNumber, WCHAR ** name)
{
	if (_ratioVector.size() == 0)
		return false;
	AcquireSRWLockExclusive(&_lock);
	auto iter = _ratioVector.rbegin();
	*outNumber = *iter;
	_ratioVector.pop_back();
	for (int i = 0; i < _itemInfo.size(); i++)
	{
		if (_itemInfo[i]->maxValue > *outNumber)
		{
			*name = _itemInfo[i]->itemName;
			break;
		}
	}
	if (!_reUse)
		_usedVector.push_back(*outNumber);
	else
	{
		_ratioVector.push_back(*outNumber);
		std::shuffle(_ratioVector.begin(), _ratioVector.end(), _m_19937);
	}
	
	ReleaseSRWLockExclusive(&_lock);
	return true;
}
