#include "ISStartup.h"
#include "ISConstants.h"
#include "ISSplashScreen.h"
#include "ISSystem.h"
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
#include "ISAutoLocking.h"
#include "ISSettings.h"
#include "ISProtocol.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISParagraphEntity.h"
#include "ISPrintingEntity.h"
#include "ISSortingBuffer.h"
#include "ISBuffer.h"
#include "ISMetaSystemsEntity.h"
#include "ISProperty.h"
#include "ISMainWindow.h"
#include "ISMediaPlayer.h"
#include "ISDeviceEntity.h"
#include "ISObjects.h"
#include "ISVersion.h"
#include "ISQueryPool.h"
//-----------------------------------------------------------------------------
static QString QS_USER_CHECK = PREPARE_QUERY("SELECT COUNT(*) "
											 "FROM _users "
											 "WHERE NOT usrs_isdeleted "
											 "AND usrs_login = :Login");
//-----------------------------------------------------------------------------
ISStartup::ISStartup() : QObject()
{

}
//-----------------------------------------------------------------------------
ISStartup::~ISStartup()
{

}
//-----------------------------------------------------------------------------
int ISStartup::Startup(const QString &UserLogin, const QString &UserPassword)
{
	ISObjects::GetInstance().Initialize();

	//�������� ��������� ������ �������������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.CheckCurrentUser"));
	ISQuery qSelectUser(QS_USER_CHECK);
	qSelectUser.BindValue(":Login", UserLogin);
	if (qSelectUser.ExecuteFirst())
	{
		if (!qSelectUser.ReadColumn("count").toInt())
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.NotFoundUserWithLogin").arg(UserLogin));
			return EXIT_FAILURE;
		}
	}

	//�������� ����-������ � ������������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.MetaDataCurrentUser"));
	ISMetaUser::Instance().Initialize(UserLogin, UserPassword);

	//������������� ����-������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.MetaData"));
	ISMetaData::GetInstanse().Initialize(ISObjects::GetInstance().GetInfo().Name, false, false);

	//������������� �������� ���� ������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.SettingsDatabase"));
	ISSettingsDatabase::GetInstance().Initialize();
	ISSettingsDatabase::GetInstance().InitializedSystemParameters();

	if (!CheckAccessDatabase(UserLogin)) //�������� ���������� ������� � ���� �������������
	{
		return EXIT_FAILURE;
	}

	if (!CheckAccessAllowed()) //�������� ���������� ������� ������������
	{
		return EXIT_FAILURE;
	}

	if (!CheckExistUserGroup()) //�������� ������� �������� ������������ � ������
	{
		return EXIT_FAILURE;
	}

	//������������� ���� �������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.UserRoles"));
	ISUserRoleEntity::GetInstance().Initialize();

	//�������� ����-������ � �������� � �����������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.SystemsAndSubSystems"));
	ISMetaSystemsEntity::GetInstance().Initialize();

	//������������� ��������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.SettingsUser"));
	ISSettings::GetInstance();

	//������������� ����������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.Paragraphs"));
	ISParagraphEntity::GetInstance();

	ISQueryPool::Instance().Start();

	//�������� ���� ��������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.PrepareQueries"));
	if (!ISQueryText::Instance().CheckAllQueries())
	{
		return EXIT_FAILURE;
	}

	//������� ����� ��������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.CommandAtStartup"));
	ISGui::ExecuteStartCommand();

	//������������� ������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.Printing"));
	ISPrintingEntity::GetInstance();

	if (!ISMetaUser::Instance().UserData->System) //���� ������������ �� ���������
	{
		if (!ISMetaUser::Instance().UserData->GroupID) //���� ������������ �������� � ������
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.UserGroupIsNull"));
			return EXIT_FAILURE;
		}
	}

	if (ISMetaUser::Instance().UserData->AccountLifeTime) //���� ��� ������������ ��������� ����������� ����� �������� ������� ������
	{
		QDate DateStart = ISMetaUser::Instance().UserData->AccountLifeTimeStart;
		QDate DateEnd = ISMetaUser::Instance().UserData->AccountLifeTimeEnd;

		if (QDate::currentDate() < DateStart)
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AccountLifetimeNotStarted"));
			return EXIT_FAILURE;
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
			return EXIT_FAILURE;
		}
	}

	//������������� �����������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.Notification"));
	ISNotifySender::GetInstance().Initialize();

	//������������� ����������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.Favorites"));
	ISFavorites::GetInstance().Initialize();

	//������������� ����������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.UserSortings"));
	ISSortingBuffer::GetInstance();

	//������������� �������� �������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.ColumnSizer"));
	if (!ISColumnSizer::Instance().Initialize())
	{
		return EXIT_FAILURE;
	}

	//������������� ������� ������������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.ExternalTools"));
	ISFastAccessEntity::GetInstance().LoadExternalTools();

	//������������� �������� �������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.CreateRecords"));
	ISFastAccessEntity::GetInstance().LoadCreateRecords();

	//������������ ���������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.InitializeDevice"));
	ISDeviceEntity::GetInstance().Initialize();

	if (!ISMetaUser::Instance().UserData->System)
	{
		if (!ISMetaUser::Instance().UserData->Birthday.isNull())
		{
			if (ISMetaUser::Instance().UserData->Birthday == QDate::currentDate())
			{
				ISSplashScreen::GetInstance().hide();
				ISMediaPlayer::GetInstance().Play(BUFFER_AUDIO("HappyBirthday"));
				ISMessageBox::ShowInformation(nullptr, LANG("HappyBirthday").arg(ISMetaUser::Instance().UserData->FullName));
			}
		}
	}

	//�������� ����� � ���������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.RegisterEnterProtocol"));
	ISProtocol::EnterApplication();

	ISObjects::GetInstance().GetInterface()->BeforeShowMainWindow();

	//�������� ������� �����
	ISSplashScreen::GetInstance().show();
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.OpeningMainWindow"));

	ISProperty::Instance().SetValue(PROPERTY_LINE_EDIT_SELECTED_MENU, SETTING_BOOL(CONST_UID_SETTING_OTHER_SELECTED_MENU));

	ISMainWindow *MainWindow = new ISMainWindow();
	ISSplashScreen::GetInstance().hide();

	if (SETTING_BOOL(CONST_UID_SETTING_VIEW_MAINWINDOWMAXIMIZE))
	{
		if (SETTING_BOOL(CONST_UID_SETTING_VIEW_STARTMAINWINDOWANIMATED))
		{
			MainWindow->ShowAnimated(true);
		}
		else
		{
			MainWindow->showMaximized();
		}
	}
	else
	{
		if (SETTING_BOOL(CONST_UID_SETTING_VIEW_STARTMAINWINDOWANIMATED))
		{
			MainWindow->ShowAnimated();
		}
		else
		{
			MainWindow->show();
		}
	}

	if (SETTING_BOOL(CONST_UID_SETTING_SECURITY_AUTOLOCK))
	{
		ISAutoLocking::GetInstance().Initialize();
		ISAutoLocking::GetInstance().SetInterval(SETTING_INT(CONST_UID_SETTING_SECURITY_AUTOLOCKINTERVAL) * 60000);
		ISAutoLocking::GetInstance().StartTimer();
		QObject::connect(MainWindow, &ISMainWindow::Unlocked, &ISAutoLocking::GetInstance(), &ISAutoLocking::UnlockApplication);
		QObject::connect(&ISAutoLocking::GetInstance(), &ISAutoLocking::AutoLock, MainWindow, &ISMainWindow::LockApplication);
	}

	MainWindow->raise();
	MainWindow->activateWindow();

	return EXIT_SUCCESS;
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
		ISSplashScreen::GetInstance().hide();
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Information.NotAccessToDatabase"));
	}

	return AccessDatabase;
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAccessAllowed()
{
	bool Result = true;

	if (!ISMetaUser::Instance().UserData->AccessAllowed) //���� � ������������ ��� ����� �������
	{
		ISSplashScreen::GetInstance().hide();
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.User.NotAccessAllowed"));

		Result = false;
	}

	return Result;
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
			ISSplashScreen::GetInstance().hide();
			ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.User.NotLinkWithGroup"));

			return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
