#include "ISCoreCenterSeven.h"
#include "ISNotifySender.h"
#include "ISSystem.h"
#include "ISPhoneNumberParser.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISMetaData.h"
#include "ISSettingsDatabase.h"
#include "ISConstants.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QS_BRANCH = PREPARE_QUERY("SELECT (SELECT ptnt_id FROM patients WHERE concat(ptnt_phonemain, ptnt_phoneother) LIKE '%' || :Phone || '%' ), brch_name, brch_administrator "
										 "FROM branches "
										 "WHERE NOT brch_isdeleted "
										 "AND brch_aliasbranch = :Alias");
//-----------------------------------------------------------------------------
ISCoreCenterSeven::ISCoreCenterSeven(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreCenterSeven::~ISCoreCenterSeven()
{

}
//-----------------------------------------------------------------------------
bool ISCoreCenterSeven::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		ISMetaData::GetInstanse().Initialize(CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION), false, false);
		ISSettingsDatabase::GetInstance().Initialize();

		AsteriskSocket = new ISAsteriskSocket(this);
		connect(AsteriskSocket, &ISAsteriskSocket::SuccessfulAuth, this, &ISCoreCenterSeven::SuccessfulAuth);
		connect(AsteriskSocket, &ISAsteriskSocket::UserEvent, this, &ISCoreCenterSeven::UserEvent);
		AsteriskSocket->Connect();
	}
	
	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreCenterSeven::SuccessfulAuth(const QStringMap &StringMap)
{
	Q_UNUSED(StringMap);
	AsteriskSocket->AddFilter(AMI_USER_EVENT);
	Started();
}
//-----------------------------------------------------------------------------
void ISCoreCenterSeven::UserEvent(const QStringMap &StringMap)
{
	QString Phone = ISPhoneNumberParser::PasteEvent(StringMap.value("CallerIDNum"));

	ISQuery qSelect(QS_BRANCH);
	qSelect.BindValue(":Alias", StringMap.value("BRANCH"));
	qSelect.BindValue(":Phone", Phone);
	if (qSelect.ExecuteFirst())
	{
		QString PatientID = qSelect.ReadColumn("ptnt_id").toString();
		QString BranchName = qSelect.ReadColumn("brch_name").toString();
		int UserID = qSelect.ReadColumn("brch_administrator").toInt();

		ISLOGGER_UNKNOWN("Incoming call from \"" + StringMap.value("CallerIDNum") + "\" to filial: " + BranchName);
		ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_INCOMING_CALL, UserID, PatientID + '_' + Phone);
	}
}
//-----------------------------------------------------------------------------
