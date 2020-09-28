#ifndef __MINLIB_SINGLETON__
#define __MINLIB_SINGLETON__
#pragma once

namespace MinLib
{
	template <class T>
	class SingleTon
	{
	protected:
		SingleTon() = default;
		~SingleTon() = default;
	public:
		SingleTon(const SingleTon&) = delete;
		SingleTon& operator = (const SingleTon&) = delete;
		static T& GetInstance()
		{
			static T instance;
			return instance;
		}
	};
}

#endif // !__MINLIB_SINGLETON__