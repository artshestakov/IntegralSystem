#include "ISStartup.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
#include "ISQuery.h"
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
#include "ISProperty.h"
#include "ISVersionInfo.h"
//-----------------------------------------------------------------------------
bool ISStartup::Startup(ISSplashScreen *SplashScreen)
{
	return CONFIG_BOOL("Protocol/Include") ? StartupNew(SplashScreen) : StartupOld(SplashScreen);
}
//-----------------------------------------------------------------------------
void ISStartup::Shutdown(ISSplashScreen *SplashScreen)
{
	ISTcpQuery qSaveMetaData(API_SAVE_META_DATA);
	qSaveMetaData.BindValue("ColumnSize", ISColumnSizer::Instance().GetColumnSize());
	qSaveMetaData.BindValue("Settings", ISSettings::Instance().GetSettingsChanged());
	if (!qSaveMetaData.Execute())
	{
		ISMessageBox::ShowCritical(SplashScreen, qSaveMetaData.GetErrorString());
	}

	ISProtocol::ExitApplication();
	if (!(SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERSORTING) ? ISSortingBuffer::Instance().SaveSortings() : ISSortingBuffer::Instance().Clear()))
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SaveSortingBuffer"), ISSortingBuffer::Instance().GetErrorString());
	}

	if (!ISFavorites::Instance().Save())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SaveFavorites"), ISFavorites::Instance().GetErrorString());
	}

	if (!ISHistory::Instance().Save())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.SaveHistory"), ISHistory::Instance().GetErrorString());
	}

	ISQueryPool::Instance().Shutdown();
	ISDatabase::Instance().DisconnectAll();
	ISGui::ExitApplication();
}
//-----------------------------------------------------------------------------
bool ISStartup::StartupOld(ISSplashScreen *SplashScreen)
{
	//Инициализация объекта конфигурации
	ISVersionInfo::Instance().SelectConfiguration("OilSphere");
	ISObjects::Instance().Initialize(ISVersionInfo::Instance().ConfigurationInfo.Name);

	//Если дата запрета меньше чем текущая - не даём зайти в программу
	QDate DateExpired = ISVersionInfo::Instance().ConfigurationInfo.DateExpired;
	if (DateExpired.isValid() && QDate::currentDate() > DateExpired)
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.ObjectDateExpired"));
		return false;
	}

	//Инициализация мета-данных
	if (!ISMetaData::Instance().Initialize(ISVersionInfo::Instance().ConfigurationInfo.Name, false, false))
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
		ISBuffer::Instance().CurrentUserInfo.Login, ISBuffer::Instance().CurrentUserInfo.Password))
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

	//Инициализация параграфов
	if (!ISParagraphEntity::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeParagraphs"), ISSettings::Instance().GetErrorString());
		return false;
	}

	ISProtocol::EnterApplication();
	ISObjects::Instance().GetInterface()->BeforeShowMainWindow();
	return true;
}
//-----------------------------------------------------------------------------
bool ISStartup::StartupNew(ISSplashScreen *SplashScreen)
{
	ISTcpQuery qAuth(API_GET_META_DATA);
	if (!qAuth.Execute()) //Не удалось получить мета-данные - выходим с ошибкой
	{
		ISMessageBox::ShowCritical(SplashScreen, qAuth.GetErrorString());
		return false;
	}
	QVariantMap MetaData = qAuth.TakeAnswer();
	ISObjects::Instance().Initialize(ISVersionInfo::Instance().ConfigurationInfo.Name);
	ISSettingsDatabase::Instance().Initialize(MetaData["SettingsDB"].toMap());
	ISUserRoleEntity::Instance().InitializeTables(MetaData["AccessTables"].toMap());
	ISUserRoleEntity::Instance().InitializeSpecial(MetaData["AccessSpecial"].toList());
	ISMetaSystemsEntity::Instance().Initialize(MetaData["SystemSubSystem"].toList());
	ISPrintingEntity::Instance().Initialize(MetaData["Printing"].toList());
	ISFavorites::Instance().Initialize(MetaData["Favorite"].toMap());
	ISHistory::Instance().Initialize(MetaData["History"].toList());
	ISSettings::Instance().Initialize(MetaData["Settings"].toList());
	ISParagraphEntity::Instance().Initialize(MetaData["Paragraphs"].toList());
	ISObjects::Instance().GetInterface()->BeforeShowMainWindow();
	if (!ISMetaData::Instance().Initialize(MetaData["MetaData"].toList())) //Не удалось проинициализировать мета-данные
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeMetaData"), ISMetaData::Instance().GetErrorString());
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAccessDatabase(ISSplashScreen *SplashScreen)
{
	if (ISBuffer::Instance().CurrentUserInfo.System)
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
