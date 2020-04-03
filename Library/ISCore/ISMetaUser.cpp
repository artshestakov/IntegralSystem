#include "ISMetaUser.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISDatabase.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_USER = PREPARE_QUERY("SELECT "
									   "usrs_issystem, "
									   "usrs_id, "
									   "usrs_group, "
									   "usrs_surname, "
									   "usrs_name, "
									   "usrs_patronymic, "
									   "usrs_birthday, "
									   "usrs_accessallowed, "
									   "usrs_accountlifetime, "
									   "usrs_accountlifetimestart, "
									   "usrs_accountlifetimeend, "
									   "usrs_fixedinactive, "
									   "usrs_inactivetimeout "
									   "FROM _users "
									   "WHERE NOT usrs_isdeleted "
									   "AND usrs_login = :Login");
//-----------------------------------------------------------------------------
static QString QS_USER_GROUP = PREPARE_QUERY("SELECT usgp_name, usgp_fullaccess FROM _usergroup WHERE NOT usgp_isdeleted AND usgp_id = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_USER_PASSWORD = PREPARE_QUERY("SELECT passwd FROM pg_shadow WHERE usename = :Login");
//-----------------------------------------------------------------------------
static QString QS_USER_STATUS = PREPARE_QUERY("SELECT usts_name "
											  "FROM _users "
											  "LEFT JOIN _userstatus ON usts_id = usrs_currentstatus "
											  "WHERE usrs_id = :UserID");
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
bool ISMetaUser::Initialize(const QString &login, const QString &password)
{
	ISQuery qSelectUser(QS_USER);
	qSelectUser.BindValue(":Login", login);
	bool Result = qSelectUser.ExecuteFirst();
	if (Result)
	{
		UserData->System = qSelectUser.ReadColumn("usrs_issystem").toBool();
		UserData->ID = qSelectUser.ReadColumn("usrs_id").toInt();
		UserData->GroupID = qSelectUser.ReadColumn("usrs_group").toInt();
		UserData->Surname = qSelectUser.ReadColumn("usrs_surname").toString();
		UserData->Name = qSelectUser.ReadColumn("usrs_name").toString();
		UserData->Patronymic = qSelectUser.ReadColumn("usrs_patronymic").toString();
		UserData->Birthday = qSelectUser.ReadColumn("usrs_birthday").toDate();
		UserData->Login = login;
		UserData->Password = password;
		UserData->IPAddress = ISDatabase::Instance().GetInetClientAddress();
		UserData->FullName = UserData->Surname + SYMBOL_SPACE + UserData->Name + SYMBOL_SPACE + UserData->Patronymic;
		UserData->AccessAllowed = qSelectUser.ReadColumn("usrs_accessallowed").toBool();
		
		UserData->AccountLifeTime = qSelectUser.ReadColumn("usrs_accountlifetime").toBool();
		UserData->AccountLifeTimeStart = qSelectUser.ReadColumn("usrs_accountlifetimestart").toDate();
		UserData->AccountLifeTimeEnd = qSelectUser.ReadColumn("usrs_accountlifetimeend").toDate();

		UserData->FixedInactive = qSelectUser.ReadColumn("usrs_fixedinactive").toBool();
		UserData->InactiveTimeout = qSelectUser.ReadColumn("usrs_inactivetimeout").toInt();
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
bool ISMetaUser::CheckPassword(const QString &EnteredPassword)
{
	ISQuery qSelectPassword(QS_USER_PASSWORD);
	qSelectPassword.BindValue(":Login", UserData->Login);
	if (qSelectPassword.ExecuteFirst())
	{
		QString MD5 = qSelectPassword.ReadColumn("passwd").toString();
		MD5 = MD5.remove(0, 3);

		QCryptographicHash CryptographicHash(QCryptographicHash::Md5);
		CryptographicHash.addData(QString(EnteredPassword + UserData->Login).toUtf8());
		QString HashString(CryptographicHash.result().toHex());

		if (MD5 == HashString)
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
QString ISMetaUser::GetCurrentStatus(int UserID) const
{
	QString Status;
	ISQuery qSelect(QS_USER_STATUS);
	qSelect.BindValue(":UserID", UserID);
	if (qSelect.ExecuteFirst())
	{
		Status = qSelect.ReadColumn("usts_name").toString();
	}

	return Status;
}
//-----------------------------------------------------------------------------