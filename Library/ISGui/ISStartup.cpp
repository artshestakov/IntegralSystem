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
#include "ISConfig.h"
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

	//�������� ���� ��������
	if (!UseProtocol)
	{
		if (!ISQueryText::Instance().CheckAllQueries())
		{
			return false;
		}
	}

	ISObjects::GetInstance().Initialize();

	//������������� ����-������
	if (!ISMetaData::GetInstanse().Initialize(ISObjects::GetInstance().GetInfo().Name, false, false))
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeMetaData"), ISMetaData::GetInstanse().GetErrorString());
		return false;
	}

	//������������� �������� ���� ������
	ISSettingsDatabase::GetInstance().Initialize();
	ISSettingsDatabase::GetInstance().InitializedSystemParameters();

	if (!CheckAccessDatabase(SplashScreen)) //�������� ���������� ������� � ���� �������������
	{
		return false;
	}

	//�������� ����-������ � ������������
	ISMetaUser::Instance().Initialize();

	if (!IsValidUser(SplashScreen))
	{
		return false;
	}

	//������������� ���� �������
	ISUserRoleEntity::GetInstance().Initialize();

	//�������� ����-������ � �������� � �����������
	ISMetaSystemsEntity::GetInstance().Initialize();

	ISQueryPool::Instance().Start();

	//������������� ������
	ISPrintingEntity::GetInstance();

	//������������� �����������
	ISNotifySender::GetInstance().Initialize();

	//������������� ����������
	ISFavorites::GetInstance().Initialize();

	//������������� ����������
	ISSortingBuffer::Instance();

	//������������� �������� �������
	if (!ISColumnSizer::Instance().Initialize(UseProtocol))
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeColumnSizer"), ISColumnSizer::Instance().GetErrorString());
		return false;
	}

	//������������� ��������������� ��������� ���� �����
	if (!LoadLocalNames(SplashScreen))
	{
		return false;
	}

	//������������� ��������
	ISSettings::GetInstance();

	//������������� ����������
	ISParagraphEntity::GetInstance();

	//������������ ���������
	ISDeviceEntity::GetInstance().Initialize();

	//�������� ����� � ���������
	ISProtocol::EnterApplication();

	ISObjects::GetInstance().GetInterface()->BeforeShowMainWindow();
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
}
//-----------------------------------------------------------------------------
bool ISStartup::IsValidUser(ISSplashScreen *SplashScreen)
{
	//�������� ��������� ������ �������������
	ISQuery qSelectUser(QS_USER_CHECK);
	qSelectUser.BindValue(":Login", ISMetaUser::Instance().UserData->Login);
	if (!qSelectUser.ExecuteFirst())
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.CheckValidUser"), qSelectUser.GetErrorString());
		return false;
	}

	//���� ����� ����� � �� �� ����������
	if (!qSelectUser.ReadColumn("count").toInt())
	{
		ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.NotFoundUserWithLogin").arg(ISMetaUser::Instance().UserData->Login));
		return false;
	}

	//���� ����� ����� ������� �� ��������
	if (qSelectUser.ReadColumn("usrs_isdeleted").toBool())
	{
		ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Error.CurrentUserIsDeleted"));
		return false;
	}

	//���� � ������������ ��� ����� �������
	if (!ISMetaUser::Instance().UserData->AccessAllowed)
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.User.NotAccessAllowed"));
		return false;
	}

	//�������� ������� �������� ������������ � ������
	if (!ISMetaUser::Instance().UserData->System)
	{
		if (!ISMetaUser::Instance().UserData->GroupID)
		{
			ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.User.NotLinkWithGroup"));
			return false;
		}
	}

	//���� ��� ������������ ��������� ����������� ����� �������� ������� ������
	if (ISMetaUser::Instance().UserData->AccountLifeTime)
	{
		QDate DateStart = ISMetaUser::Instance().UserData->AccountLifeTimeStart;
		QDate DateEnd = ISMetaUser::Instance().UserData->AccountLifeTimeEnd;
		if (QDate::currentDate() < DateStart)
		{
			ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.AccountLifetimeNotStarted"));
			return false;
		}
		else if (QDate::currentDate() == DateEnd) //���� ������� �������� ���� ��������
		{
			ISMessageBox::ShowWarning(SplashScreen, LANG("Message.Warning.AccountLifetimeLeftLastDay"));
		}
		else if (QDate::currentDate().addDays(1) == DateEnd) //���� ������ �������� ���� ��������
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
	else
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.OverrideLocalListName"), qSelect.GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
