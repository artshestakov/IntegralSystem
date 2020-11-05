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
#include "ISObjects.h"
#include "ISQueryPool.h"
#include "ISProperty.h"
#include "ISConfig.h"
#include "ISHistory.h"
#include "ISDatabase.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
bool ISStartup::Startup(ISSplashScreen *SplashScreen)
{
	return CONFIG_BOOL("Protocol/Use") ? StartupNew(SplashScreen) : StartupOld(SplashScreen);
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

	if (!ISSettings::Instance().Save())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SaveUserSettings"), ISSettings::Instance().GetErrorString());
	}

	ISQueryPool::Instance().Shutdown();
	ISDatabase::Instance().DisconnectAll();
	ISGui::ExitApplication();
}
//-----------------------------------------------------------------------------
bool ISStartup::StartupOld(ISSplashScreen *SplashScreen)
{
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
	if (!ISQueryText::Instance().CheckAllQueries())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.CheckAllQuery"), ISQueryText::Instance().GetErrorString());
		return false;
	}

	//Инициализация настроек базы данных
	if (!ISSettingsDatabase::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SelectSettingsDatabase"), ISSettingsDatabase::Instance().GetErrorString());
		return false;
	}

	if (!CheckAccessDatabase(SplashScreen)) //Проверка разрешения доступа к базе пользователям
	{
		return false;
	}

	//Загрузка мета-данных о пользователе
	if (!ISMetaUser::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, ISMetaUser::Instance().GetErrorString());
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

	if (!ISQueryPool::Instance().Start(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
		ISMetaUser::Instance().UserData.Login, ISMetaUser::Instance().UserData.Password))
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeQueryPool"), ISQueryPool::Instance().GetErrorString());
		return false;
	}

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
	if (!ISColumnSizer::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeColumnSizer"), ISColumnSizer::Instance().GetErrorString());
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
	PROPERTY_SET(PROPERTY_LINE_EDIT_SELECTED_MENU, SETTING_BOOL(CONST_UID_SETTING_OTHER_SELECTED_MENU));
	return true;
}
//-----------------------------------------------------------------------------
bool ISStartup::StartupNew(ISSplashScreen *SplashScreen)
{
	ISTcpQuery qAuth(API_GET_META_DATA);
	bool Result = qAuth.Execute();
	if (Result)
	{
		ISMetaSystemsEntity::Instance().Initialize(qAuth.GetAnswer()["SystemSubSystem"].toList());
		ISPrintingEntity::Instance().Initialize(qAuth.GetAnswer()["Printing"].toList());
		ISFavorites::Instance().Initialize(qAuth.GetAnswer()["Favorite"].toMap());
		ISHistory::Instance().Initialize(qAuth.GetAnswer()["History"].toList());
		ISSortingBuffer::Instance().Initialize(qAuth.GetAnswer()["Sorting"].toList());
		ISColumnSizer::Instance().Initialize(qAuth.GetAnswer()["ColumnSize"].toList());
		ISParagraphEntity::Instance().Initialize(qAuth.GetAnswer()["Paragraphs"].toList());
	}
	else
	{
		ISMessageBox::ShowCritical(SplashScreen, qAuth.GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAccessDatabase(ISSplashScreen *SplashScreen)
{
	if (ISMetaUser::Instance().UserData.System)
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
