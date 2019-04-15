#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISApplicationRunning
{
public:
	ISApplicationRunning(const QString &key);
	~ISApplicationRunning();

	bool IsAnotherRunning();
	bool TryToRun();
	void Release();

private:
	const QString Key;
	const QString MemoryLockKey;
	const QString SharedMemoryKey;

	QSharedMemory SharedMemory;
	QSystemSemaphore SystemSemaphore;

	Q_DISABLE_COPY(ISApplicationRunning)
};
//-----------------------------------------------------------------------------
