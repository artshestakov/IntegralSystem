#include "ISStartup.h"
#include "ISConstants.h"
#include "ISSplashScreen.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISQueryText.h"
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
//-----------------------------------------------------------------------------
static QString QS_USER_CHECK = PREPARE_QUERY("SELECT COUNT(*) "
											 "FROM _users "
											 "WHERE NOT usrs_isdeleted "
											 "AND usrs_login = :Login");
//-----------------------------------------------------------------------------
static QString Q_SET_APPLICATION_NAME = "SET application_name = 'IntegralSystem %1'";
//-----------------------------------------------------------------------------
static QString QS_ALREADY_ONLINE = PREPARE_QUERY("SELECT COUNT(*) "
												 "FROM pg_stat_activity "
												 "WHERE usename = :Login");
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

	//��������� ����� ���������� � ���� ������
	SetApplicationNameQuery();

	//�������� ����-������ � ������������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.MetaDataCurrentUser"));
	ISMetaUser::GetInstance().Initialize(UserLogin, UserPassword);

	//������������� ����-������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.MetaData"));
	ISMetaData::GetInstanse().Initialize(/*ISLicense::GetInstance().GetName()*/"", false, false); //???

	//������������� �������� ���� ������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.SettingsDatabase"));
	ISSettingsDatabase::GetInstance().Initialize();
	ISSettingsDatabase::GetInstance().InitializedSystemParameters();

	if (!CheckAccessDatabase(UserLogin)) //�������� ���������� ������� � ���� �������������
	{
		return ExitError();
	}

	if (!CheckAccessAllowed()) //�������� ���������� ������� ������������
	{
		return ExitError();
	}

	if (!CheckExistUserGroup()) //�������� ������� �������� ������������ � ������
	{
		return ExitError();
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

	//�������� ���� ��������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.PrepareQueries"));
	ISQueryText::GetInstance().CheckAllQueries();

	//������� ����� ��������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.CommandAtStartup"));
	ISCore::ExecuteStartCommand();

	//������������� ������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.Printing"));
	ISPrintingEntity::GetInstance();

	if (!ISMetaUser::GetInstance().GetData()->System) //���� ������������ �� ���������
	{
		if (!ISMetaUser::GetInstance().GetData()->GroupID) //���� ������������ �������� � ������
		{
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.UserGroupIsNull"));
			return EXIT_FAILURE;
		}
	}

	if (ISMetaUser::GetInstance().GetData()->AccountLifetime) //���� ��� ������������ ��������� ����������� ����� �������� ������� ������
	{
		QDate DateStart = ISMetaUser::GetInstance().GetData()->AccountLifetimeStart;
		QDate DateEnd = ISMetaUser::GetInstance().GetData()->AccountLifetimeEnd;

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
	ISColumnSizer::GetInstance().Initialize();

	//������������� ������� ������������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.ExternalTools"));
	ISFastAccessEntity::GetInstance().LoadExternalTools();

	//������������� �������� �������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.CreateRecords"));
	ISFastAccessEntity::GetInstance().LoadCreateRecords();

	//������������ ���������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.InitializeDevice"));
	ISDeviceEntity::GetInstance().Initialize();

	if (!ISMetaUser::GetInstance().GetData()->System)
	{
		if (CheckAlreadyConnected())
		{
			ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_ALREADY_CONNECTED, ISMetaUser::GetInstance().GetData()->ID, ISMetaUser::GetInstance().GetData()->IPAddress, QString(), false);
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.AlreadyConnected").arg(ISMetaUser::GetInstance().GetData()->Login));
			ISSystem::ExecLoop(1500);
			return EXIT_FAILURE;
		}

		if (!ISMetaUser::GetInstance().GetData()->Birthday.isNull())
		{
			if (ISMetaUser::GetInstance().GetData()->Birthday == QDate::currentDate())
			{
				ISSplashScreen::GetInstance().hide();
				ISMediaPlayer::GetInstance().Play(BUFFER_AUDIO("HappyBirthday"));
				ISMessageBox::ShowInformation(nullptr, LANG("HappyBirthday").arg(ISMetaUser::GetInstance().GetData()->FullName));
			}
		}
	}

	//�������� ����� � ���������
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.RegisterEnterProtocol"));
	ISProtocol::EnterApplication();

	ISObjects::GetInstance().Initialize();
	ISObjects::GetInstance().GetInterface()->BeforeShowMainWindow();

	//�������� ������� �����
	ISSplashScreen::GetInstance().show();
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.OpeningMainWindow"));

	ISProperty::GetInstance().SetValue(IS_PROPERTY_LINE_EDIT_SELECTED_MENU, SETTING_BOOL(CONST_UID_SETTING_OTHER_SELECTED_MENU));

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
int ISStartup::ExitNormal()
{
	ISGui::ExitApplication();
	return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
int ISStartup::ExitError()
{
	ISGui::ExitApplication();
	return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
void ISStartup::SetApplicationNameQuery()
{
	ISQuery qSetApplicationName;
	if (!qSetApplicationName.Execute(QString(Q_SET_APPLICATION_NAME).arg(ISVersion::GetInstance().GetVersion())))
	{
		ISMessageBox::ShowCritical(nullptr, qSetApplicationName.GetErrorText());
	}
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAlreadyConnected()
{
	bool Result = false;
	ISQuery qSelectConnected(QS_ALREADY_ONLINE);
	qSelectConnected.BindValue(":Login", ISMetaUser::GetInstance().GetData()->Login);
	if (qSelectConnected.ExecuteFirst())
	{
		int Count = qSelectConnected.ReadColumn("count").toInt();
		if (Count > 1)
		{
			Result = true;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISStartup::CheckAccessDatabase(const QString &Login)
{
	if (ISMetaUser::GetInstance().GetData()->System)
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

	if (!ISMetaUser::GetInstance().GetData()->AccessAllowed) //���� � ������������ ��� ����� �������
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
	if (ISMetaUser::GetInstance().GetData()->System)
	{
		return true;
	}
	else
	{
		if (!ISMetaUser::GetInstance().GetData()->GroupID)
		{
			ISSplashScreen::GetInstance().hide();
			ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.User.NotLinkWithGroup"));

			return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
