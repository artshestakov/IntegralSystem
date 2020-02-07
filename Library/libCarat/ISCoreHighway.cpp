#include "ISCoreHighway.h"
#include "ISDebug.h"
#include "ISQuery.h"
#include "ISPhoneNumberParser.h"
#include "ISNotifySender.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_ORGANIZATION_USER = PREPARE_QUERY("SELECT orgz_id, orgz_name, orgz_user, aspt_pattern "
													"FROM organization "
													"LEFT JOIN _asteriskpattern ON aspt_user = orgz_user "
													"WHERE orgz_id = (SELECT orph_organization FROM organizationphone WHERE NOT orph_isdeleted AND orph_phone = :Phone LIMIT 1)");
//-----------------------------------------------------------------------------
static QString QI_RAITING = PREPARE_QUERY("INSERT INTO raiting(rtng_user, rtng_raiting) "
										  "VALUES((SELECT aspt_user FROM _asteriskpattern WHERE aspt_pattern = :Pattern), :Raiting) "
										  "RETURNING userfullname(rtng_user)");
//-----------------------------------------------------------------------------
ISCoreHighway::ISCoreHighway(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreHighway::~ISCoreHighway()
{

}
//-----------------------------------------------------------------------------
bool ISCoreHighway::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		AsteriskSocket = new ISAsteriskSocket(this);
		connect(AsteriskSocket, &ISAsteriskSocket::SuccessfulAuth, this, &ISCoreHighway::SuccessfulAuth);
		connect(AsteriskSocket, &ISAsteriskSocket::UserEvent, this, &ISCoreHighway::UserEvent);
		AsteriskSocket->Connect();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreHighway::SuccessfulAuth(const QStringMap &StringMap)
{
	Q_UNUSED(StringMap);
	AsteriskSocket->AddFilter(AMI_USER_EVENT);
	Started();
}
//-----------------------------------------------------------------------------
void ISCoreHighway::UserEvent(const QStringMap &StringMap)
{
	QStringList StringList = StringMap.value("UserEvent").split(SYMBOL_SPACE);
	QString EventType = StringList.at(0);
	if (EventType == "call") //Событие входящего звонка
	{
		QString Number = StringMap.value("CallerIDNum");
		ISDebug::ShowInfoString("Incoming call from number \"" + Number + "\", search organization...");

		ISQuery qSelect(QS_ORGANIZATION_USER);
		qSelect.BindValue(":Phone", ISPhoneNumberParser::PasteEvent(StringList.at(1)));
		if (qSelect.ExecuteFirst())
		{
			int OrganizationID = qSelect.ReadColumn("orgz_id").toInt();
			QString OrganizationName = qSelect.ReadColumn("orgz_name").toString();
			int UserID = qSelect.ReadColumn("orgz_user").toInt();
			QString Pattern = qSelect.ReadColumn("aspt_pattern").toString();

			ISDebug::ShowInfoString("Organization founded. Redirect call to number: " + Pattern);
			AsteriskSocket->Redirect(StringMap, Pattern);
			ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_INCOMING_CALL, UserID, OrganizationID);
		}
		else
		{
			ISDebug::ShowInfoString("Organization not found");
		}
	}
	else if (EventType == "rating") //Событие оценки качества
	{
		int Rating = StringList.at(1).toInt(); //Оценка
		QString Pattern = StringList.at(2); //Внутренний номер

		ISQuery qInsertRaiting(QI_RAITING);
		qInsertRaiting.BindValue(":Raiting", Rating);
		qInsertRaiting.BindValue(":Pattern", Pattern);
		if (qInsertRaiting.ExecuteFirst())
		{
			ISDebug::ShowString("New rating \"" + QString::number(Rating) + "\" for user: " + qInsertRaiting.ReadColumn("userfullname").toString());
		}
	}
}
//-----------------------------------------------------------------------------
