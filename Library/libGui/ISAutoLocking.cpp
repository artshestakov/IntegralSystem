#include "ISAutoLocking.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISNotificationService.h"
#include "ISMediaPlayer.h"
//-----------------------------------------------------------------------------
ISAutoLocking::ISAutoLocking()
	: QObject(),
	Lock(false),
	Initialized(false)
{
	
}
//-----------------------------------------------------------------------------
ISAutoLocking::~ISAutoLocking()
{

}
//-----------------------------------------------------------------------------
ISAutoLocking& ISAutoLocking::GetInstance()
{
	static ISAutoLocking LockingApplication;
	return LockingApplication;
}
//-----------------------------------------------------------------------------
void ISAutoLocking::Initialize()
{
	TimerLock = new QTimer(this);
	connect(TimerLock, &QTimer::timeout, this, &ISAutoLocking::Timeout);

	Initialized = true;
}
//-----------------------------------------------------------------------------
void ISAutoLocking::StartTimer()
{
	TimerLock->start();
}
//-----------------------------------------------------------------------------
void ISAutoLocking::RestartTimer()
{
	TimerLock->start();
}
//-----------------------------------------------------------------------------
void ISAutoLocking::SetInterval(int interval)
{
	TimerLock->setInterval(interval);
}
//-----------------------------------------------------------------------------
bool ISAutoLocking::GetLock() const
{
	return Lock;
}
//-----------------------------------------------------------------------------
bool ISAutoLocking::GetInitialized() const
{
	return Initialized;
}
//-----------------------------------------------------------------------------
void ISAutoLocking::LockApplication()
{
	Lock = true;
}
//-----------------------------------------------------------------------------
void ISAutoLocking::UnlockApplication()
{
	Lock = false;
}
//-----------------------------------------------------------------------------
void ISAutoLocking::Timeout()
{
	if (!Lock)
	{
		ISMediaPlayer::GetInstance().Play(BUFFER_AUDIO("Locking"));

		ISNotificationService::ShowNotification(LANG("AutoLocking"));
		LockApplication();
		emit AutoLock();
	}
}
//-----------------------------------------------------------------------------
