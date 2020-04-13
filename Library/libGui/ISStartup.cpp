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
//-----------------------------------------------------------------------------
static QString QS_USER_CHECK = PREPARE_QUERY("SELECT COUNT(*) "
											 "FROM _users "
											 "WHERE NOT usrs_isdeleted "
											 "AND usrs_login = :Login");
//-----------------------------------------------------------------------------
static QString QS_LOCAL_NAME = PREPARE_QUERY("SELECT lcnm_tablename, lcnm_fieldname, lcnm_localname "
											 "FROM _localnames "
											 "WHERE lcnm_user = currentuserid()");
//-----------------------------------------------------------------------------
ISStartup::ISStartup() : QObject()
{

}
//-----------------------------------------------------------------------------
ISStartup::~ISStartup()
{

}
//-----------------------------------------------------------------------------
bool ISStartup::Startup(ISSplashScreen *SplashScreen, const QString &UserLogin, const QString &UserPassword)
{
	//�������� ���� ��������
	if (!ISQueryText::Instance().CheckAllQueries())
	{
		return false;
	}

	ISObjects::GetInstance().Initialize();

	//�������� ��������� ������ �������������
	ISQuery qSelectUser(QS_USER_CHECK);
	qSelectUser.BindValue(":Login", UserLogin);
	if (qSelectUser.ExecuteFirst())
	{
		if (!qSelectUser.ReadColumn("count").toInt())
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.NotFoundUserWithLogin").arg(UserLogin));
			return false;
		}
	}

	//�������� ����-������ � ������������
	ISMetaUser::Instance().Initialize(UserLogin, UserPassword);

	//������������� ����-������
	ISMetaData::GetInstanse().Initialize(ISObjects::GetInstance().GetInfo().Name, false, false);

	//������������� �������� ���� ������
	ISSettingsDatabase::GetInstance().Initialize();
	ISSettingsDatabase::GetInstance().InitializedSystemParameters();

	if (!CheckAccessDatabase(UserLogin)) //�������� ���������� ������� � ���� �������������
	{
		return false;
	}

	if (!CheckAccessAllowed()) //�������� ���������� ������� ������������
	{
		return false;
	}

	if (!CheckExistUserGroup()) //�������� ������� �������� ������������ � ������
	{
		return false;
	}

	if (!LoadLocalNames())
	{
		return false;
	}

	//������������� ���� �������
	ISUserRoleEntity::GetInstance().Initialize();

	//�������� ����-������ � �������� � �����������
	ISMetaSystemsEntity::GetInstance().Initialize();

	//������������� ��������
	ISSettings::GetInstance();

	//������������� ����������
	ISParagraphEntity::GetInstance();

	ISQueryPool::Instance().Start();

	//������������� ������
	ISPrintingEntity::GetInstance();

	if (!ISMetaUser::Instance().UserData->System) //���� ������������ �� ���������
	{
		if (!ISMetaUser::Instance().UserData->GroupID) //���� ������������ �������� � ������
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.UserGroupIsNull"));
			return false;
		}
	}

	if (ISMetaUser::Instance().UserData->AccountLifeTime) //���� ��� ������������ ��������� ����������� ����� �������� ������� ������
	{
		QDate DateStart = ISMetaUser::Instance().UserData->AccountLifeTimeStart;
		QDate DateEnd = ISMetaUser::Instance().UserData->AccountLifeTimeEnd;

		if (QDate::currentDate() < DateStart)
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeNotStarted"));
			return false;
		}
		else if (QDate::currentDate() == DateEnd) //���� ������� �������� ���� ��������
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeLeftLastDay"));
		}
		else if (QDate::currentDate().addDays(1) == DateEnd) //���� ������ �������� ���� ��������
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeLeftLastDayTwo"));
		}
		else if (QDate::currentDate() > DateEnd)
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeEnded"));
			return false;
		}
	}

	//������������� �����������
	ISNotifySender::GetInstance().Initialize();

	//������������� ����������
	ISFavorites::GetInstance().Initialize();

	//������������� ����������
	ISSortingBuffer::Instance();

	//������������� �������� �������
	if (!ISColumnSizer::Instance().Initialize())
	{
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.InitializeColumnSizer"), ISColumnSizer::Instance().GetErrorString());
		return false;
	}

	//������������� ������� ������������
	ISFastAccessEntity::GetInstance().LoadExternalTools();

	//������������� �������� �������
	ISFastAccessEntity::GetInstance().LoadCreateRecords();

	//������������ ���������
	ISDeviceEntity::GetInstance().Initialize();

	//�������� ����� � ���������
	ISProtocol::EnterApplication();

	ISObjects::GetInstance().GetInterface()->BeforeShowMainWindow();
	ISProperty::Instance().SetValue(PROPERTY_LINE_EDIT_SELECTED_MENU, SETTING_BOOL(CONST_UID_SETTING_OTHER_SELECTED_MENU));
	return true;
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAccessDatabase(const QString &Login)
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
	if (!ISMetaUser::Instance().UserData->AccessAllowed) //���� � ������������ ��� ����� �������
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
