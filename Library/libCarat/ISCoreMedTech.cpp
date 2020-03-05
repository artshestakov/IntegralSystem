#include "ISCoreMedTech.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISMetaData.h"
#include "ISSettingsDatabase.h"
#include "ISConstants.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QI_RATING = PREPARE_QUERY("INSERT INTO rating(rtng_uniqueid, rtng_datetimecall, rtng_branch, rtng_rating) "
										 "VALUES(:UniqueID, now(), (SELECT brnm_branch FROM branchesnumber WHERE brnm_number = :InteriorNumber), :Rating) "
										 "RETURNING rtng_id");
//-----------------------------------------------------------------------------
static QString QS_BRANCH = PREPARE_QUERY("SELECT brch_name FROM branches WHERE brch_id = (SELECT rtng_branch FROM rating WHERE rtng_id = :RatingID)");
//-----------------------------------------------------------------------------
ISCoreMedTech::ISCoreMedTech(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreMedTech::~ISCoreMedTech()
{

}
//-----------------------------------------------------------------------------
bool ISCoreMedTech::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		ISMetaData::GetInstanse().Initialize(/*CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION)*/"", false, false); //???
		ISSettingsDatabase::GetInstance().Initialize();

		AsteriskSocket = new ISAsteriskSocket(this);
		connect(AsteriskSocket, &ISAsteriskSocket::SuccessfulAuth, this, &ISCoreMedTech::SuccessfulAuth);
		connect(AsteriskSocket, &ISAsteriskSocket::UserEvent, this, &ISCoreMedTech::UserEvent);
		AsteriskSocket->Connect();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreMedTech::SuccessfulAuth(const QStringMap &StringMap)
{
	Q_UNUSED(StringMap);
	AsteriskSocket->AddFilter(AMI_USER_EVENT);
	Started();
}
//-----------------------------------------------------------------------------
void ISCoreMedTech::UserEvent(const QStringMap &StringMap)
{
	QString StringUserEvent = StringMap.value("UserEvent");
	QStringList StringList = StringUserEvent.split(SYMBOL_SPACE);
	int InteriorNumber = 0;
	
	int Rating = StringList.at(0).toInt(); //Оценка
	QString Pattern = StringList.at(1); //Внутренний номер
	if (Pattern.contains("SIP"))
	{
		InteriorNumber = Pattern.split('/').at(1).toInt();
	}
	else
	{
		InteriorNumber = Pattern.toInt();
	}

	ISQuery qInsert(QI_RATING);
	qInsert.BindValue(":UniqueID", StringMap.value("Uniqueid"));
	qInsert.BindValue(":InteriorNumber", InteriorNumber);
	qInsert.BindValue(":Rating", Rating);
	if (qInsert.ExecuteFirst())
	{
		int RatingID = qInsert.ReadColumn("rtng_id").toInt();

		ISQuery qSelectBranch(QS_BRANCH);
		qSelectBranch.BindValue(":RatingID", RatingID);
		if (qSelectBranch.ExecuteFirst())
		{
			ISLOGGER_UNKNOWN("New raiting \"" + QString::number(Rating) + "\" for filial: " + qSelectBranch.ReadColumn("brch_name").toString());
		}
	}
}
//-----------------------------------------------------------------------------
