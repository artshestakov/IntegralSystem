#pragma once
#ifndef _ISSINGLETON_H_INCLUDED
#define _ISSINGLETON_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISSingleton
{
public:
	static ISSingleton& Instance()
	{
		static ISSingleton Singleton;
		return Singleton;
	}

private:
	ISSingleton() = default;
	~ISSingleton() = default;
	
	ISSingleton(const ISSingleton &) = delete;
	ISSingleton& operator=(const ISSingleton &) = delete;
	
	void* operator new(std::size_t) = delete;
	void* operator new[](std::size_t) = delete;
	
	void operator delete(void *) = delete;
	void operator delete[](void *) = delete;
};
//-----------------------------------------------------------------------------
#endif
