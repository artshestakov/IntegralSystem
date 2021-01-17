#include "ISFail2Ban.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISFail2Ban::ISFail2Ban() : QObject()
{
	Timer = new QTimer(this);
	connect(Timer, &QTimer::timeout, this, &ISFail2Ban::Timeout);
	
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISFail2Ban::~ISFail2Ban()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISFail2Ban& ISFail2Ban::Instance()
{
	static ISFail2Ban Fail2Ban;
	return Fail2Ban;
}
//-----------------------------------------------------------------------------
void ISFail2Ban::Start()
{
	Timer->start(30000);
}
//-----------------------------------------------------------------------------
bool ISFail2Ban::IsLock(const QString &IPAddress)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool Result = false;
	for (const ISFailAuthInfo &FailAuthInfo : Vector)
	{
		if (FailAuthInfo.IPAddress == IPAddress)
		{
			Result = FailAuthInfo.IsLock;
			break;
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return Result;
}
//-----------------------------------------------------------------------------
QDateTime ISFail2Ban::GetUnlockDateTime(const QString &IPAddress)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	QDateTime DateTime;
	for (const ISFailAuthInfo &FailAuthInfo : Vector)
	{
		if (FailAuthInfo.IPAddress == IPAddress)
		{
			DateTime = FailAuthInfo.DTUnlock;
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return DateTime;
}
//-----------------------------------------------------------------------------
bool ISFail2Ban::Add(const QString &IPAddress)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool is_found = false, is_lock = false;
	for (ISFailAuthInfo &FailAuthInfo : Vector) //»щем такой адрес в списке
	{
		if (FailAuthInfo.IPAddress == IPAddress && !FailAuthInfo.IsLock) //Ќашли - регистрируем временную метку
		{
			is_found = true;

			QDateTime current_date_time = QDateTime::currentDateTime();
			FailAuthInfo.DTPoints.emplace_back(current_date_time);

			is_lock = FailAuthInfo.DTPoints.size() == CARAT_BAN_ATTEMPT_COUNT;
			FailAuthInfo.IsLock = is_lock;
			if (is_lock) //≈сли адрес заблокирован - рассчитываем дата и врем€ разблокировки
			{
				FailAuthInfo.DTUnlock = current_date_time.addSecs(3600);
			}
			break;
		}
	}
	
	if (!is_found) //Ќе нашли - добавл€ем
	{
		Vector.emplace_back(ISFailAuthInfo{ IPAddress, false, { QDateTime::currentDateTime() } });
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return is_lock;
}
//-----------------------------------------------------------------------------
void ISFail2Ban::Remove(const QString &IPAddress)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	for (size_t i = 0, c = Vector.size(); i < c; ++i)
	{
		if (Vector[i].IPAddress == IPAddress)
		{
			Vector.erase(Vector.begin() + i);
			break;
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISFail2Ban::Timeout()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	for (ISFailAuthInfo &FailAuthInfo : Vector)
	{
		//≈сли текуща€ дата и врем€ больше чем врем€ разблокировки - удал€ем из списка
		if (QDateTime::currentDateTime() > FailAuthInfo.DTUnlock)
		{
			ISLOGGER_I(__CLASS__, "Address " + FailAuthInfo.IPAddress + " unlocked");
			Remove(FailAuthInfo.IPAddress);
			break;
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
