#include "ISStartup.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISNotifySender.h"
#include "ISFavorites.h"
#include "ISColumnSizer.h"
#include "ISUserRoleEntity.h"
#include "ISSettingsDatabase.h"
#include "ISFastAccessEntity.h"
#include "ISSettings.h"
#include "ISProtocol.h"
#include "ISGui.h"
#include "ISParagraphEntity.h"
#include "ISPrintingEntity.h"
#include "ISSortingBuffer.h"
#include "ISBuffer.h"
#include "ISMetaSystemsEntity.h"
#include "ISProperty.h"
#include "ISMediaPlayer.h"
#include "ISDeviceEntity.h"
#include "ISObjects.h"
#include "ISQueryPool.h"
#include "ISProperty.h"
//-----------------------------------------------------------------------------
static QString QS_USER_CHECK = PREPARE_QUERY("SELECT COUNT(*) "
											 "FROM _users "
											 "WHERE NOT usrs_isdeleted "
											 "AND usrs_login = :Login");
//-----------------------------------------------------------------------------
static QString QS_LOCAL_NAME = PREPARE_QUERY("SELECT lcnm_tablename, lcnm_fieldname, lcnm_localname "
											 "FROM _localnames "
											 "WHERE NOT lcnm_isdeleted "
											 "AND lcnm_user = currentuserid()");
//-----------------------------------------------------------------------------
ISStartup::ISStartup() : QObject()
{

}
//-----------------------------------------------------------------------------
ISStartup::~ISStartup()
{

}
//-----------------------------------------------------------------------------
bool ISStartup::Startup()
{
	//Проверка всех запросов
	if (!ISQueryText::Instance().CheckAllQueries())
	{
		return false;
	}

	ISObjects::GetInstance().Initialize();

	//Проверка введенных данных пользователем
	ISQuery qSelectUser(QS_USER_CHECK);
	qSelectUser.BindValue(":Login", ISMetaUser::Instance().UserData->Login);
	if (qSelectUser.ExecuteFirst())
	{
		if (!qSelectUser.ReadColumn("count").toInt())
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.NotFoundUserWithLogin").arg(ISMetaUser::Instance().UserData->Login));
			return false;
		}
	}

	//Загрузка мета-данных о пользователе
	ISMetaUser::Instance().Initialize();

	//Инициализация мета-данных
	ISMetaData::GetInstanse().Initialize(ISObjects::GetInstance().GetInfo().Name, false, false);

	//Инициализация настроек базы данных
	ISSettingsDatabase::GetInstance().Initialize();
	ISSettingsDatabase::GetInstance().InitializedSystemParameters();

	if (!CheckAccessDatabase()) //Проверка разрешения доступа к базе пользователям
	{
		return false;
	}

	if (!CheckAccessAllowed()) //Проверка разрешения доступа пользователя
	{
		return false;
	}

	if (!CheckExistUserGroup()) //Проверка наличия привязки пользователя к группе
	{
		return false;
	}

	if (!LoadLocalNames())
	{
		return false;
	}

	//Инициализация прав доступа
	ISUserRoleEntity::GetInstance().Initialize();

	//Загрузка мета-данных о системах и подсистемах
	ISMetaSystemsEntity::GetInstance().Initialize();

	//Инициализация настроек
	ISSettings::GetInstance();

	//Инициализация параграфов
	ISParagraphEntity::GetInstance();

	ISQueryPool::Instance().Start();

	//Инициалищация печати
	ISPrintingEntity::GetInstance();

	if (!ISMetaUser::Instance().UserData->System) //Если пользователь НЕ СИСТЕМНЫЙ
	{
		if (!ISMetaUser::Instance().UserData->GroupID) //Если пользователь привязан к группе
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.UserGroupIsNull"));
			return false;
		}
	}

	if (ISMetaUser::Instance().UserData->AccountLifeTime) //Если для пользователя настроено ограничение срока действия учётной записи
	{
		QDate DateStart = ISMetaUser::Instance().UserData->AccountLifeTimeStart;
		QDate DateEnd = ISMetaUser::Instance().UserData->AccountLifeTimeEnd;

		if (QDate::currentDate() < DateStart)
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeNotStarted"));
			return false;
		}
		else if (QDate::currentDate() == DateEnd) //Если сегодня истекает срок действия
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeLeftLastDay"));
		}
		else if (QDate::currentDate().addDays(1) == DateEnd) //Если завтра истекает срок действия
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeLeftLastDayTwo"));
		}
		else if (QDate::currentDate() > DateEnd)
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeEnded"));
			return false;
		}
	}

	//Инициализация нотификаций
	ISNotifySender::GetInstance().Initialize();

	//Инициализация избранного
	ISFavorites::GetInstance().Initialize();

	//Инициализация сортировок
	ISSortingBuffer::Instance();

	//Инициализация размеров колонок
	if (!ISColumnSizer::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.InitializeColumnSizer"), ISColumnSizer::Instance().GetErrorString());
		return false;
	}

	//Инициализация внешних инструментов
	ISFastAccessEntity::GetInstance().LoadExternalTools();

	//Инициализация создания записей
	ISFastAccessEntity::GetInstance().LoadCreateRecords();

	//Иницилазация устройств
	ISDeviceEntity::GetInstance().Initialize();

	//Фиксация входа в протоколе
	ISProtocol::EnterApplication();

	ISObjects::GetInstance().GetInterface()->BeforeShowMainWindow();
	ISProperty::Instance().SetValue(PROPERTY_LINE_EDIT_SELECTED_MENU, SETTING_BOOL(CONST_UID_SETTING_OTHER_SELECTED_MENU));
	return true;
}
//-----------------------------------------------------------------------------
void ISStartup::Shutdown()
{
	ISProtocol::ExitApplication();
	if (!(SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERSORTING) ? ISSortingBuffer::Instance().SaveSortings() : ISSortingBuffer::Instance().Clear()))
	{
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.SaveSortingBuffer"), ISSortingBuffer::Instance().GetErrorString());
	}

	if (!(SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERCOLUMNSIZE) ? ISColumnSizer::Instance().Save() : ISColumnSizer::Instance().Clear()))
	{
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.SaveColumnSizer"), ISColumnSizer::Instance().GetErrorString());
	}
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAccessDatabase()
{
	if (ISMetaUser::Instance().UserData->System)
	{
		return true;
	}

	bool AccessDatabase = SETTING_DATABASE_VALUE_BOOL(CONST_UID_DATABASE_SETTING_GENERAL_ACCESSDATABASE);
	if (!AccessDatabase)
	{
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.NotAccessToDatabase"));
	}
	return AccessDatabase;
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAccessAllowed()
{
	if (!ISMetaUser::Instance().UserData->AccessAllowed) //Если у пользователя нет права доступа
	{
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.User.NotAccessAllowed"));
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckExistUserGroup()
{
	if (ISMetaUser::Instance().UserData->System)
	{
		return true;
	}
	else
	{
		if (!ISMetaUser::Instance().UserData->GroupID)
		{
			ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.User.NotLinkWithGroup"));
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISStartup::LoadLocalNames()
{
	ISQuery qSelect(QS_LOCAL_NAME);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(qSelect.ReadColumn("lcnm_tablename").toString());
			if (MetaTable)
			{
				PMetaField *MetaField = MetaTable->GetField(qSelect.ReadColumn("lcnm_fieldname").toString());
				if (MetaField)
				{
					MetaField->LabelName = qSelect.ReadColumn("lcnm_localname").toString();
					MetaField->LocalListName = MetaField->LabelName;
				}
			}

		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
