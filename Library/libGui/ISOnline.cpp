#include "ISOnline.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISQueryPool.h"
//-----------------------------------------------------------------------------
static QString QI_ONLINE = PREPARE_QUERY("INSERT INTO _useronline(uonl_type) "
										 "SELECT uotp_id FROM _useronlinetype WHERE uotp_uid = :TypeUID");
//-----------------------------------------------------------------------------
ISOnline::ISOnline() : QObject()
{
	Timer = new QTimer(this);
	connect(Timer, &QTimer::timeout, this, &ISOnline::Timeout);
}
//-----------------------------------------------------------------------------
ISOnline::~ISOnline()
{

}
//-----------------------------------------------------------------------------
ISOnline& ISOnline::GetInstance()
{
	static ISOnline Online;
	return Online;
}
//-----------------------------------------------------------------------------
void ISOnline::Initialize(int Timeout)
{
	Timer->setInterval(Timeout * 60000);
	qApp->installEventFilter(this);
	Enter();
}
//-----------------------------------------------------------------------------
void ISOnline::Enter()
{
	Insert(true, CONST_UID_USER_ONLINE_TYPE_ENTER);
	Timer->start();
}
//-----------------------------------------------------------------------------
void ISOnline::Exit()
{
	Timer->stop();
	Insert(false, CONST_UID_USER_ONLINE_TYPE_EXIT);
}
//-----------------------------------------------------------------------------
void ISOnline::Timeout()
{
	Timer->stop();
	Insert(true, CONST_UID_USER_ONLINE_TYPE_INACTIVE); //Регистрация не активности
}
//-----------------------------------------------------------------------------
bool ISOnline::eventFilter(QObject *Watched, QEvent *e)
{
	if (e->type() == QEvent::Enter || 
		e->type() == QEvent::Leave ||
		e->type() == QEvent::MouseMove ||
		e->type() == QEvent::MouseButtonPress ||
		e->type() == QEvent::Show ||
		e->type() == QEvent::Hide ||
		e->type() == QEvent::WindowActivate ||
		e->type() == QEvent::WindowDeactivate ||
		e->type() == QEvent::Move ||
		e->type() == QEvent::KeyPress)
	{
		if (!Timer->isActive()) //Если во время активности таймер был выключен - зарегистрировать активность
		{
			Insert(true, CONST_UID_USER_ONLINE_TYPE_ACTIVE);
		}
		Timer->start();
	}

	return QObject::eventFilter(Watched, e);
}
//-----------------------------------------------------------------------------
void ISOnline::Insert(bool Thread, const ISUuid &TypeUID)
{
	if (Thread)
	{
		QVariantMap VariantMap;
		VariantMap.insert(":TypeUID", TypeUID);
		ISQueryPool::Instance().AddQuery(QI_ONLINE, VariantMap);
	}
	else
	{
		ISQuery qInsert(QI_ONLINE);
		qInsert.BindValue(":TypeUID", TypeUID);
		qInsert.Execute();
	}
}
//-----------------------------------------------------------------------------
