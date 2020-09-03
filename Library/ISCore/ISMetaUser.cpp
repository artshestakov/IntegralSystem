#include "ISMetaUser.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_USER = PREPARE_QUERY("SELECT "
									   "usrs_issystem, "
									   "usrs_id, "
									   "usrs_oid, "
									   "usrs_group, "
									   "usrs_fio, "
									   "usrs_birthday, "
									   "usrs_accessallowed, "
									   "usrs_accountlifetime, "
									   "usrs_accountlifetimestart, "
									   "usrs_accountlifetimeend "
									   "FROM _users "
									   "WHERE NOT usrs_isdeleted "
									   "AND usrs_login = :Login");
//-----------------------------------------------------------------------------
static QString QS_USER_GROUP = PREPARE_QUERY("SELECT usgp_name, usgp_fullaccess FROM _usergroup WHERE NOT usgp_isdeleted AND usgp_id = :GroupID");
//-----------------------------------------------------------------------------
ISMetaUser::ISMetaUser()
	: UserData(new ISMetaUserData()),
	ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISMetaUser::~ISMetaUser()
{
	delete UserData;
}
//-----------------------------------------------------------------------------
ISMetaUser& ISMetaUser::Instance()
{
	static ISMetaUser UserData;
	return UserData;
}
//-----------------------------------------------------------------------------
QString ISMetaUser::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISMetaUser::Initialize()
{
	ISQuery qSelectUser(QS_USER);
	qSelectUser.BindValue(":Login", UserData->Login);
	bool Result = qSelectUser.ExecuteFirst();
	if (Result)
	{
		UserData->System = qSelectUser.ReadColumn("usrs_issystem").toBool();
		UserData->ID = qSelectUser.ReadColumn("usrs_id").toInt();
		UserData->OID = qSelectUser.ReadColumn("usrs_oid").toInt();
		UserData->GroupID = qSelectUser.ReadColumn("usrs_group").toInt();
		UserData->FIO = qSelectUser.ReadColumn("usrs_fio").toString();
		UserData->Birthday = qSelectUser.ReadColumn("usrs_birthday").toDate();
		UserData->AccessAllowed = qSelectUser.ReadColumn("usrs_accessallowed").toBool();
		UserData->AccountLifeTime = qSelectUser.ReadColumn("usrs_accountlifetime").toBool();
		UserData->AccountLifeTimeStart = qSelectUser.ReadColumn("usrs_accountlifetimestart").toDate();
		UserData->AccountLifeTimeEnd = qSelectUser.ReadColumn("usrs_accountlifetimeend").toDate();
	}
	else
	{
		ErrorString = qSelectUser.GetErrorString();
	}

	if (Result)
	{
		ISQuery qSelectGroup(QS_USER_GROUP);
		qSelectGroup.BindValue(":GroupID", UserData->GroupID);
		Result = qSelectGroup.ExecuteFirst();
		if (Result)
		{
			UserData->GroupName = qSelectGroup.ReadColumn("usgp_name").toString();
			UserData->GroupFullAccess = qSelectGroup.ReadColumn("usgp_fullaccess").toBool();
		}
		else
		{
			ErrorString = qSelectGroup.GetErrorString();
		}
	}
	
	return Result;
}
//-----------------------------------------------------------------------------
