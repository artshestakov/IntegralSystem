#include "ISMetaUser.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISConstants.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_USER = PREPARE_QUERY("SELECT "
									   "usrs_issystem, "
									   "usrs_id, "
									   "usrs_group, "
									   "usrs_fio, "
									   "usrs_birthday, "
									   "usrs_accessallowed, "
									   "usrs_accountlifetime, "
									   "usrs_accountlifetimestart, "
									   "usrs_accountlifetimeend, "
									   "usgp_name, "
									   "usgp_fullaccess "
									   "FROM _users "
									   "LEFT JOIN _usergroup ON usgp_id = usrs_group "
									   "WHERE usrs_login = :Login");
//-----------------------------------------------------------------------------
ISMetaUser::ISMetaUser()
	: ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISMetaUser::~ISMetaUser()
{

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
	//Запрашиваем информацию о пользователе
	ISQuery qSelectUser(QS_USER);
	qSelectUser.BindValue(":Login", UserData.Login);
	if (!qSelectUser.Execute()) //Не удалось выполнить запрос
	{
		ErrorString = LANG("Message.Error.MetaUserSelect").arg(qSelectUser.GetErrorString());
		return false;
	}
	
	//Не удалось перейти к первой записи: учётная запись существует в БД, но отсутствует в таблице _Users
	if (!qSelectUser.First())
	{
		ErrorString = LANG("Message.Error.NotFoundUserWithLogin").arg(ISMetaUser::Instance().UserData.Login);
		return false;
	}
	
	//Заполняем структуру данными
	UserData.System = qSelectUser.ReadColumn("usrs_issystem").toBool();
	UserData.ID = qSelectUser.ReadColumn("usrs_id").toInt();
	UserData.GroupID = qSelectUser.ReadColumn("usrs_group").toInt();
	UserData.FIO = qSelectUser.ReadColumn("usrs_fio").toString();
	UserData.GroupFullAccess = qSelectUser.ReadColumn("usgp_fullaccess").toBool();

	//Если у пользователя нет права доступа
	if (!qSelectUser.ReadColumn("usrs_accessallowed").toBool())
	{
		ErrorString = LANG("Message.Error.User.NotAccessAllowed");
		return false;
	}

	//Проверка наличия привязки пользователя к группе
	if (!UserData.System && !UserData.GroupID)
	{
		ErrorString = LANG("Message.Error.User.NotLinkWithGroup");
		return false;
	}

	//Если для пользователя настроено ограничение срока действия учётной записи
	if (qSelectUser.ReadColumn("usrs_accountlifetime").toBool())
	{
		if (QDate::currentDate() < qSelectUser.ReadColumn("usrs_accountlifetimestart").toDate())
		{
			ErrorString = LANG("Message.Warning.AccountLifetimeNotStarted");
			return false;
		}
		else if (QDate::currentDate() > qSelectUser.ReadColumn("usrs_accountlifetimeend").toDate())
		{
			ErrorString = LANG("Message.Warning.AccountLifetimeEnded");
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
