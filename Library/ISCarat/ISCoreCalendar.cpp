#include "ISCoreCalendar.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISNotifySender.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_CALENDAR = PREPARE_QUERY("SELECT cldr_id, cldr_user, userfullname(cldr_user), cldr_name "
										   "FROM _calendar "
										   "WHERE NOT cldr_closed "
										   "AND cldr_date = CURRENT_DATE "
										   "AND to_char(cldr_timealert, 'HH24:MI') = :TimeAlert");
//-----------------------------------------------------------------------------
ISCoreCalendar::ISCoreCalendar(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreCalendar::~ISCoreCalendar()
{

}
//-----------------------------------------------------------------------------
bool ISCoreCalendar::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		Timer = new QTimer(this);
		Timer->setInterval(60000);
		Timer->setSingleShot(true);
		connect(Timer, &QTimer::timeout, this, &ISCoreCalendar::Timeout);
		Timer->start();
		Started();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreCalendar::Timeout()
{
	ISQuery qSelectEvents(QS_CALENDAR);
	qSelectEvents.BindValue(":TimeAlert", QString("%1:%2").arg(QTime::currentTime().hour()).arg(QTime::currentTime().minute()));
	if (qSelectEvents.Execute())
	{
		while (qSelectEvents.Next())
		{
			int EventID = qSelectEvents.ReadColumn("cldr_id").toInt();
            //int UserID = qSelectEvents.ReadColumn("cldr_user").toInt();
			QString UserName = qSelectEvents.ReadColumn("userfullname").toString();
			QString Name = qSelectEvents.ReadColumn("cldr_name").toString();

			//ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_CALENDAR_EVENT, UserID, EventID, Name, false);
			ISLOGGER_L("Sended reminder \"" + QString::number(EventID) + "\" for: " + UserName);
		}
	}
	Timer->start();
}
//-----------------------------------------------------------------------------
