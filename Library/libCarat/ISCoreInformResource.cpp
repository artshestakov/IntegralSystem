#include "ISCoreInformResource.h"
#include "ISConstants.h"
#include "ISNotifySender.h"
#include "ISSystem.h"
#include "ISPhoneNumberParser.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISMetaData.h"
#include "ISSettingsDatabase.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QS_ORGANIZATION = PREPARE_QUERY("SELECT orgz_id, orgz_user, aspt_pattern, userfullname(orgz_user) "
											   "FROM organizations "
											   "LEFT JOIN _asteriskpattern ON aspt_user = orgz_user "
											   "WHERE concat(orgz_principalphone, orgz_principalphonework, orgz_chiefaccountantphone, orgz_chiefaccountantphonework, orgz_departmentlegalphone, orgz_departmentlegalphonework, orgz_departmentframesphone, orgz_departmentframesphonework, orgz_chiefagronomistphone, orgz_chiefagronomistphonework, orgz_chiefengineerphone, orgz_chiefengineerphonework, orgz_departmentsalesphone, orgz_departmentsalesphonework) LIKE '%' || :Phone || '%' "
											   "AND NOT orgz_isdeleted "
											   "ORDER BY orgz_id");
//-----------------------------------------------------------------------------
ISCoreInformResource::ISCoreInformResource(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreInformResource::~ISCoreInformResource()
{

}
//-----------------------------------------------------------------------------
bool ISCoreInformResource::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		ISMetaData::GetInstanse().Initialize(/*CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION)*/"", false, false); //???
		ISSettingsDatabase::GetInstance().Initialize();

		AsteriskSocket = new ISAsteriskSocket(this);
		connect(AsteriskSocket, &ISAsteriskSocket::SuccessfulAuth, this, &ISCoreInformResource::SuccessfulAuth);
		connect(AsteriskSocket, &ISAsteriskSocket::UserEvent, this, &ISCoreInformResource::UserEvent);
		AsteriskSocket->Connect();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreInformResource::SuccessfulAuth(const QStringMap &StringMap)
{
	Q_UNUSED(StringMap);
	AsteriskSocket->AddFilter(AMI_USER_EVENT);
	Started();
}
//-----------------------------------------------------------------------------
void ISCoreInformResource::UserEvent(const QStringMap &StringMap)
{
	QString Number = ISPhoneNumberParser::PasteEvent(StringMap.value("CallerIDNum"));
	ISLOGGER_UNKNOWN("Search organization with number: " + Number);

	ISQuery qSelect(QS_ORGANIZATION);
	qSelect.BindValue(":Phone", Number);
	if (qSelect.ExecuteFirst())
	{
		int OrganizationID = qSelect.ReadColumn("orgz_id").toInt();
		int UserID = qSelect.ReadColumn("orgz_user").toInt();
		if (UserID)
		{
			QString Pattern = qSelect.ReadColumn("aspt_pattern").toString();
			if (Pattern.length())
			{
				QString UserName = qSelect.ReadColumn("userfullname").toString();

				ISLOGGER_UNKNOWN("Organization founded. Redirect to: " + UserName);
				AsteriskSocket->Redirect(StringMap, Pattern);
				ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_INCOMING_CALL, UserID, OrganizationID);
			}
		}
		else
		{
			ISLOGGER_UNKNOWN("Organization not found");
		}
	}
}
//-----------------------------------------------------------------------------
