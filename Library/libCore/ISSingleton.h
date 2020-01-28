#pragma once
#ifndef _ISSINGLETON_H_INCLUDED
#define _ISSINGLETON_H_INCLUDED
//-----------------------------------------------------------------------------
template<class T> class ISSingleton
{
public:
	static T* Instance(void)
	{
		if (!instance)
		{
			instance = new T();
		}
		return instance;
	}

	static void Destroy(void)
	{
		if (instance)
		{
			delete instance;
			instance = nullptr;
		}
	}

protected:
	ISSingleton()
	{

	}

private:
	static T* instance;
};
//-----------------------------------------------------------------------------
template<class T> T* ISSingleton<T>::instance = nullptr;
//-----------------------------------------------------------------------------
#endif
