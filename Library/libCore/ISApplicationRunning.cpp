#include "ISApplicationRunning.h"
//-----------------------------------------------------------------------------
ISApplicationRunning::ISApplicationRunning(const std::string &key)
	: Key(key)
{
	
}
//-----------------------------------------------------------------------------
ISApplicationRunning::~ISApplicationRunning()
{
	
}
//-----------------------------------------------------------------------------
bool ISApplicationRunning::IsRunning()
{
	bool Result = true;
#ifdef WIN32
	HANDLE Mutex = CreateMutex(NULL, TRUE, Key.c_str());
	Result = GetLastError() == 0;
	if (!Result)
	{
		ReleaseMutex(Mutex);
		CloseHandle(Mutex);
	}
#else
	int RC = flock(open(PATH_LOCK_FILE.toStdString().c_str(), O_CREAT | O_RDWR, 0666), LOCK_EX | LOCK_NB);
	if (!RC)
	{
		Result = false;
	}
#endif
	return Result;
}
//-----------------------------------------------------------------------------
