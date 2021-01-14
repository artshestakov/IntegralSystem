#include "ISStartup.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
#include "ISFavorites.h"
#include "ISColumnSizer.h"
#include "ISUserRoleEntity.h"
#include "ISSettingsDatabase.h"
#include "ISSettings.h"
#include "ISGui.h"
#include "ISParagraphEntity.h"
#include "ISBuffer.h"
#include "ISMetaSystemsEntity.h"
#include "ISObjects.h"
#include "ISConfig.h"
#include "ISDatabase.h"
#include "ISTcpQuery.h"
#include "ISVersionInfo.h"
//-----------------------------------------------------------------------------
bool ISStartup::Startup(ISSplashScreen *SplashScreen)
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
	ISFavorites::Instance().Initialize(MetaData["Favorite"].toMap());
	ISColumnSizer::Instance().Initialize(MetaData["ColumnSize"].toMap());
	ISSettings::Instance().Initialize(MetaData["Settings"].toList());
	ISParagraphEntity::Instance().Initialize(MetaData["Paragraphs"].toList());
	ISBuffer::Instance().SetTaskPriority(MetaData["TaskPriority"].toList());
	ISObjects::Instance().GetInterface()->BeforeShowMainWindow();
	if (!ISMetaData::Instance().Initialize(MetaData["MetaData"].toList())) //Не удалось проинициализировать мета-данные
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeMetaData"), ISMetaData::Instance().GetErrorString());
		return false;
	}
	ISObjects::Instance().GetInterface()->BeforeShowMainWindow();
	return true;
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
	ISDatabase::Instance().DisconnectAll();
	ISGui::ExitApplication();
}
//-----------------------------------------------------------------------------
