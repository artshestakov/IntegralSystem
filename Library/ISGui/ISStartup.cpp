#include "ISStartup.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISFavorites.h"
#include "ISColumnSizer.h"
#include "ISUserRoleEntity.h"
#include "ISSettingsDatabase.h"
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
#include "ISObjects.h"
#include "ISQueryPool.h"
#include "ISProperty.h"
#include "ISConfig.h"
#include "ISHistory.h"
//-----------------------------------------------------------------------------
static QString QS_USER_CHECK = PREPARE_QUERY("SELECT "
											 "(SELECT COUNT(*) FROM _users WHERE usrs_login = :Login), "
											 "(SELECT usrs_isdeleted FROM _users WHERE usrs_login = :Login)");
//-----------------------------------------------------------------------------
static QString QS_LOCAL_NAME = PREPARE_QUERY("SELECT lcnm_tablename, lcnm_fieldname, lcnm_localname "
											 "FROM _localnames "
											 "WHERE NOT lcnm_isdeleted "
											 "AND lcnm_user = currentuserid()");
//-----------------------------------------------------------------------------
bool ISStartup::Startup(ISSplashScreen *SplashScreen)
{
	bool UseProtocol = CONFIG_BOOL("Protocol/Use");

	//Инициализация объекта конфигурации
	if (!ISObjects::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeObjects"), ISObjects::Instance().GetErrorString());
		return false;
	}

	//Если дата запрета меньше чем текущая - не даём зайти в программу
	QDate DateExpired = ISObjects::Instance().Info.DateExpired;
	if (DateExpired.isValid() && QDate::currentDate() > DateExpired)
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.ObjectDateExpired"));
		return false;
	}

	//Инициализация мета-данных
	if (!ISMetaData::Instance().Initialize(ISObjects::Instance().Info.Name, false, false))
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeMetaData"), ISMetaData::Instance().GetErrorString());
		return false;
	}

	//Проверка всех запросов
	if (!UseProtocol)
	{
		if (!ISQueryText::Instance().CheckAllQueries())
		{
			ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.CheckAllQuery"), ISQueryText::Instance().GetErrorString());
			return false;
		}
	}

	//Инициализация настроек базы данных
	ISSettingsDatabase::GetInstance().Initialize();

	if (!CheckAccessDatabase(SplashScreen)) //Проверка разрешения доступа к базе пользователям
	{
		return false;
	}

	//Загрузка мета-данных о пользователе
	ISMetaUser::Instance().Initialize();
	if (!IsValidUser(SplashScreen))
	{
		return false;
	}

	//Инициализация прав доступа
	if (!ISUserRoleEntity::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeUserRole"), ISUserRoleEntity::Instance().GetErrorString());
		return false;;
	}

	//Загрузка мета-данных о системах и подсистемах
	if (!ISMetaSystemsEntity::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeMetaSystems"), ISMetaSystemsEntity::Instance().GetErrorString());
		return false;
	}

	ISQueryPool::Instance().Start();
	
	//Инициалищация печати
	if (!ISPrintingEntity::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializePrinting"), ISPrintingEntity::Instance().GetErrorString());
		return false;
	}

	//Инициализация избранного
	if (!ISFavorites::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeFavorites"), ISFavorites::Instance().GetErrorString());
		return false;
	}

	//Инициализация истории
	if (!ISHistory::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeHistory"), ISHistory::Instance().GetErrorString());
		return false;
	}

	//Инициализация сортировок
	if (!ISSortingBuffer::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeSortingBuffer"), ISSortingBuffer::Instance().GetErrorString());
		return false;
	}

	//Инициализация размеров колонок
	if (!ISColumnSizer::Instance().Initialize(UseProtocol))
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeColumnSizer"), ISColumnSizer::Instance().GetErrorString());
		return false;
	}

	//Инициализация переопределнных локальных имен полей
	if (!LoadLocalNames(SplashScreen))
	{
		return false;
	}

	//Инициализация настроек пользователя
	if (!ISSettings::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeUserSettings"), ISSettings::Instance().GetErrorString());
		return false;
	}

	//Инициализация параграфов
	if (!ISParagraphEntity::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeParagraphs"), ISSettings::Instance().GetErrorString());
		return false;
	}

	ISProtocol::EnterApplication();
	ISObjects::Instance().GetInterface()->BeforeShowMainWindow();
	ISProperty::Instance().SetValue(PROPERTY_LINE_EDIT_SELECTED_MENU, SETTING_BOOL(CONST_UID_SETTING_OTHER_SELECTED_MENU));
	return true;
}
//-----------------------------------------------------------------------------
void ISStartup::Shutdown(ISSplashScreen *SplashScreen)
{
	ISProtocol::ExitApplication();
	if (!(SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERSORTING) ? ISSortingBuffer::Instance().SaveSortings() : ISSortingBuffer::Instance().Clear()))
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SaveSortingBuffer"), ISSortingBuffer::Instance().GetErrorString());
	}

	if (!(SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERCOLUMNSIZE) ? ISColumnSizer::Instance().Save() : ISColumnSizer::Instance().Clear()))
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SaveColumnSizer"), ISColumnSizer::Instance().GetErrorString());
	}

	if (!ISFavorites::Instance().Save())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SaveFavorites"), ISFavorites::Instance().GetErrorString());
	}

	if (!ISHistory::Instance().Save())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SaveHistory"), ISHistory::Instance().GetErrorString());
	}
	ISGui::ExitApplication();
}
//-----------------------------------------------------------------------------
bool ISStartup::IsValidUser(ISSplashScreen *SplashScreen)
{
	//Проверка введенных данных пользователем
	ISQuery qSelectUser(QS_USER_CHECK);
	qSelectUser.BindValue(":Login", ISMetaUser::Instance().UserData->Login);
	if (!qSelectUser.ExecuteFirst())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.CheckValidUser"), qSelectUser.GetErrorString());
		return false;
	}

	//Если такой логин в БД не существует
	if (!qSelectUser.ReadColumn("count").toInt())
	{
		ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.NotFoundUserWithLogin").arg(ISMetaUser::Instance().UserData->Login));
		return false;
	}

	//Если такой логин помечен на удаление
	if (qSelectUser.ReadColumn("usrs_isdeleted").toBool())
	{
		ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Error.CurrentUserIsDeleted"));
		return false;
	}

	//Если у пользователя нет права доступа
	if (!ISMetaUser::Instance().UserData->AccessAllowed)
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.User.NotAccessAllowed"));
		return false;
	}

	//Проверка наличия привязки пользователя к группе
	if (!ISMetaUser::Instance().UserData->System)
	{
		if (!ISMetaUser::Instance().UserData->GroupID)
		{
			ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.User.NotLinkWithGroup"));
			return false;
		}
	}

	//Если для пользователя настроено ограничение срока действия учётной записи
	if (ISMetaUser::Instance().UserData->AccountLifeTime)
	{
		QDate DateStart = ISMetaUser::Instance().UserData->AccountLifeTimeStart;
		QDate DateEnd = ISMetaUser::Instance().UserData->AccountLifeTimeEnd;
		if (QDate::currentDate() < DateStart)
		{
			ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.AccountLifetimeNotStarted"));
			return false;
		}
		else if (QDate::currentDate() == DateEnd) //Если сегодня истекает срок действия
		{
			ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.AccountLifetimeLeftLastDay"));
		}
		else if (QDate::currentDate().addDays(1) == DateEnd) //Если завтра истекает срок действия
		{
			ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.AccountLifetimeLeftLastDayTwo"));
		}
		else if (QDate::currentDate() > DateEnd)
		{
			ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.AccountLifetimeEnded"));
			return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAccessDatabase(ISSplashScreen *SplashScreen)
{
	if (ISMetaUser::Instance().UserData->System)
	{
		return true;
	}

	bool AccessDatabase = SETTING_DATABASE_VALUE_BOOL(CONST_UID_DATABASE_SETTING_GENERAL_ACCESSDATABASE);
	if (!AccessDatabase)
	{
		ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.NotAccessToDatabase"));
	}
	return AccessDatabase;
}
//-----------------------------------------------------------------------------
bool ISStartup::LoadLocalNames(ISSplashScreen *SplashScreen)
{
	ISQuery qSelect(QS_LOCAL_NAME);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(qSelect.ReadColumn("lcnm_tablename").toString());
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
	else
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.OverrideLocalListName"), qSelect.GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
