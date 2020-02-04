#include "ISApplicationRunning.h"
//-----------------------------------------------------------------------------
QString GenerateKeyHash(const QString &key, const QString &salt)
{
	QByteArray ByteArray;
	ByteArray.append(key.toUtf8());
	ByteArray.append(salt.toUtf8());
	ByteArray = QCryptographicHash::hash(ByteArray, QCryptographicHash::Sha1).toHex();
	return ByteArray;
}
//-----------------------------------------------------------------------------
ISApplicationRunning::ISApplicationRunning(const QString &key)
	: Key(key),
	MemoryLockKey(GenerateKeyHash(key, "_memLockKey")),
	SharedMemoryKey(GenerateKeyHash(key, "_sharedmemKey")),
	SharedMemory(SharedMemoryKey),
	SystemSemaphore(MemoryLockKey, 1)
{
	SystemSemaphore.acquire();
	{
		QSharedMemory fix(SharedMemoryKey);
		fix.attach();
	}
	SystemSemaphore.release();
}
//-----------------------------------------------------------------------------
ISApplicationRunning::~ISApplicationRunning()
{
	Release();
}
//-----------------------------------------------------------------------------
bool ISApplicationRunning::IsAnotherRunning()
{
	if (SharedMemory.isAttached())
	{
		return false;
	}

	SystemSemaphore.acquire();
	const bool isRunning = SharedMemory.attach();
	if (isRunning)
	{
		SharedMemory.detach();
	}

	SystemSemaphore.release();
	return isRunning;
}
//-----------------------------------------------------------------------------
bool ISApplicationRunning::TryToRun()
{
	if (IsAnotherRunning())
	{
		return false;
	}

	SystemSemaphore.acquire();
	const bool result = SharedMemory.create(sizeof(quint64));
	SystemSemaphore.release();
	if (!result)
	{
		Release();
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISApplicationRunning::Release()
{
	SystemSemaphore.acquire();
	if (SharedMemory.isAttached())
	{
		SharedMemory.detach();
	}
	SystemSemaphore.release();
}
//-----------------------------------------------------------------------------
