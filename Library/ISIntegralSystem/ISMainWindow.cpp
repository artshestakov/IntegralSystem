#include "StdAfx.h"
#include "ISMainWindow.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISProtocol.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISSortingBuffer.h"
#include "ISMetaUser.h"
#include "ISSettings.h"
#include "ISFavoritesForm.h"
#include "ISAboutForm.h"
#include "ISSettingsDatabase.h"
#include "ISControls.h"
#include "ISNoteForm.h"
#include "ISDatabase.h"
#include "ISSplashScreen.h"
#include "ISUserRoleEntity.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISMetaSystemsEntity.h"
#include "ISLockForm.h"
#include "ISExitForm.h"
#include "ISMemoryObjects.h"
#include "ISColumnSizer.h"
#include "ISHistoryForm.h"
#include "ISCalculatorForm.h"
#include "ISListBaseForm.h"
#include "ISFastCreateObjectForm.h"
#include "ISExternalToolsForm.h"
#include "ISInputDialog.h"
#include "ISUserPasswordForm.h"
#include "ISParagraphEntity.h"
#include "ISNotifyRecipient.h"
#include "ISNotifySender.h"
#include "ISFileDialog.h"
#include "ISNotificationService.h"
#include "ISCreatedObjectsEntity.h"
#include "ISFeedbackForm.h"
#include "ISLogger.h"
#include "ISListViewWidget.h"
#include "ISIncomingCallBaseForm.h"
#include "ISUserStatusForm.h"
#include "ISStyleSheet.h"
#include "ISDeviceSettingsForm.h"
#include "ISTelephony.h"
#include "ISLicense.h"
#include "ISObjects.h"
#include "ISDebug.h"
#include "ISOnline.h"
#include "ISLicenseForm.h"
#include "ISScreenshot.h"
#include "ISAddressBookListForm.h"
//-----------------------------------------------------------------------------
ISMainWindow::ISMainWindow(QWidget *parent) : ISInterfaceForm(parent)
{
	CloseEvent = true;

	ISMemoryObjects::GetInstance().SetMainWindow(this);

	connect(&ISCreatedObjectsEntity::GetInstance(), &ISCreatedObjectsEntity::Existed, this, &ISMainWindow::ActivateWorkspace);

	setWindowIcon(BUFFER_PIXMAPS("Logo"));
	setWindowTitle(QString("IntegralSystem - %1 : %2").arg(ISLicense::GetInstance().GetLocalName()).arg(ISMetaUser::GetInstance().GetData()->FullName));
	resize(SIZE_MAIN_WINDOW);
	setMinimumSize(SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setSpacing(0);

	CreateMenuBar();
	CreateInformationMessage();
	CreateStackWidget();
	CreateStatusBar();

	SystemTrayIcon = new QSystemTrayIcon(this);
	SystemTrayIcon->setIcon(BUFFER_ICONS("Logo"));
	SystemTrayIcon->setToolTip(LOCALIZATION("ClickedFromExpandApplicationTray"));
	connect(SystemTrayIcon, &QSystemTrayIcon::activated, this, &ISMainWindow::SystemTrayActivated);

	PhoneForm = new ISPhoneForm();

	//Отключение пользователя
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::TermianteUser, this, &ISMainWindow::TerminateMe);

	//Доступно обновление
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::UpdateAviable, this, &ISMainWindow::UpdateAviable);

	//Входящий звонок
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::IncomingCall, this, &ISMainWindow::IncomingCall);

	//Сообщение о аналогичном подключении
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::AlreadyConnected, this, &ISMainWindow::AlreadyConnected);

	//Получение рабочего стола
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::GetScreenshot, &ISScreenshot::GetInstance(), &ISScreenshot::Create);
}
//-----------------------------------------------------------------------------
ISMainWindow::~ISMainWindow()
{

}
//-----------------------------------------------------------------------------
void ISMainWindow::SetCloseEventFlag(bool close_event)
{
	CloseEvent = close_event;
}
//-----------------------------------------------------------------------------
bool ISMainWindow::GetCloseEventFlag() const
{
	return CloseEvent;
}
//-----------------------------------------------------------------------------
void ISMainWindow::SetCurrentParagraphUID(const ISUuid &current_paragraph_uid)
{
	CurrentParagraphUID = current_paragraph_uid;
}
//-----------------------------------------------------------------------------
ISUuid ISMainWindow::GetCurrentParagraphUID() const
{
	return CurrentParagraphUID;
}
//-----------------------------------------------------------------------------
void ISMainWindow::LockApplication()
{
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_LOCK_APPLICATION, QString(), QString(), QVariant());
	HideAnimation();

	ISLockForm *LockForm = new ISLockForm();
	connect(LockForm, &ISLockForm::Unlocked, [=]
	{
		SetVisibleShadow(false);
		show();
		ShowAnimated();
		raise();
		activateWindow();
		setFocus();

		emit Unlocked();
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_UNLOCK_APPLICATION, QString(), QString(), QVariant());
	});
	LockForm->ShowAnimated();
}
//-----------------------------------------------------------------------------
void ISMainWindow::closeEvent(QCloseEvent *e)
{
	if (CloseEvent)
	{
		if (ISCreatedObjectsEntity::GetInstance().CheckExistForms())
		{
			if (SETTING_BOOL(CONST_UID_SETTING_SECURITY_PASSWORDEXITSHOW))
			{
				QString Password = ISInputDialog::GetPassword(this, LOCALIZATION("Security"), LOCALIZATION("EnterThePasswordForExit")).toString();
				if (Password.length())
				{
					if (Password != SETTING_STRING(CONST_UID_SETTING_SECURITY_PASSWORDEXIT))
					{
						e->ignore();
						ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Password.Error"));
						return;
					}
				}
				else
				{
					e->ignore();
					return;
				}
			}

			if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_CONFIRMEXITAPPLICATION))
			{
				SetVisibleShadow(true);
				raise();
				activateWindow();

				ISSystem::SetWaitGlobalCursor(true);
				ISExitForm ExitForm;
				ExitForm.raise();
				connect(&ExitForm, &ISExitForm::Showed, [=] { ISSystem::SetWaitGlobalCursor(false); });

				if (ExitForm.Exec())
				{
					switch (ExitForm.GetSelectedAction())
					{
					case ISNamespace::EFA_Lock:
						e->ignore();
						LockApplication();
						break;

					case ISNamespace::EFA_Tray:
						e->ignore();
						TrayClicked();
						break;

					case ISNamespace::EFA_ChangeUser:
						e->ignore();
						BeforeClose();
						ISCore::ChangeUser();
						break;

					case ISNamespace::EFA_Exit:
						BeforeClose();
						ISCore::ExitApplication();
						break;

					case ISNamespace::EFT_Close:
						e->ignore();
						break;
					}
				}
				else
				{
					e->ignore();
				}

				SetVisibleShadow(false);
			}
			else
			{
				BeforeClose();
				ISCore::ExitApplication();
			}
		}
		else
		{
			e->ignore();
		}
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::AfterShowEvent()
{
	ISInterfaceForm::AfterShowEvent();

	if (SETTING_BOOL(CONST_UID_SETTING_VIEW_FULLSCREEN))
	{
		setWindowState(Qt::WindowFullScreen);
	}

	QTimer::singleShot(3000, this, &ISMainWindow::InitializePlugin);
	
	int NewNotifyCount = ISNotifySender::GetInstance().GetNewNotify();
	if (NewNotifyCount)
	{
		MenuBar->GetbuttonNotify()->SetCountNotify(NewNotifyCount);
		ISNotificationService::ShowNotification(LOCALIZATION("NewNotifications").arg(NewNotifyCount));
		QApplication::beep();
	}

	if (ISMetaUser::GetInstance().GetData()->FixedInactive)
	{
		ISOnline::GetInstance().Initialize(ISMetaUser::GetInstance().GetData()->InactiveTimeout);
	}

	ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_USER_ONLINE, QVariant(), CURRENT_USER_FULL_NAME, false);

	int CountOverdue = ISCore::TaskCountOverdue();
	if (CountOverdue)
	{
		ISNotificationService::ShowNotification(LOCALIZATION("YouHaveExpiredTasks").arg(CountOverdue));
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::CreateMenuBar()
{
	MenuBar = new ISMenuBar(this);
	connect(MenuBar, &ISMenuBar::ParagraphClicked, this, &ISMainWindow::ParagraphClicked);
	connect(MenuBar, &ISMenuBar::CreateRecords, this, &ISMainWindow::ShowCreateRecords);
	connect(MenuBar, &ISMenuBar::ExternalTools, this, &ISMainWindow::ShowExternalTools);
	connect(MenuBar, &ISMenuBar::Lock, this, &ISMainWindow::LockApplication);
	connect(MenuBar, &ISMenuBar::ChangeUser, this, &ISMainWindow::ChangeUser);
	connect(MenuBar, &ISMenuBar::Exit, this, &ISMainWindow::close);
	connect(MenuBar, &ISMenuBar::Favorites, this, &ISMainWindow::ShowFavoritesForm);
	connect(MenuBar, &ISMenuBar::History, this, &ISMainWindow::ShowHistoryForm);
	connect(MenuBar, &ISMenuBar::DeviceSettings, this, &ISMainWindow::ShowDeviceSettings);
	connect(MenuBar, &ISMenuBar::ChangePassword, this, &ISMainWindow::ShowChangePasswordForm);
	connect(MenuBar, &ISMenuBar::CreateLogToday, this, &ISMainWindow::CreateLogToday);
	connect(MenuBar, &ISMenuBar::Debug, this, &ISMainWindow::ShowDebugApplication);
	connect(MenuBar, &ISMenuBar::UserStatus, this, &ISMainWindow::UserStatusChange);
	connect(MenuBar, &ISMenuBar::Settings, this, &ISMainWindow::ShowSettingsForm);
	connect(MenuBar, &ISMenuBar::Notebook, this, &ISMainWindow::ShowNoteForm);
	connect(MenuBar, &ISMenuBar::Calculator, this, &ISMainWindow::ShowCalculator);
	connect(MenuBar, &ISMenuBar::AddressBook, this, &ISMainWindow::ShowAddressBook);
	connect(MenuBar, &ISMenuBar::AboutApplication, this, &ISMainWindow::ShowAboutForm);
	connect(MenuBar, &ISMenuBar::FeedbackForm, this, &ISMainWindow::ShowFeedbackForm);
	connect(MenuBar, &ISMenuBar::AboutQt, this, &ISMainWindow::ShowAboutQt);
	connect(MenuBar, &ISMenuBar::License, this, &ISMainWindow::ShowLicenseForm);
	GetMainLayout()->addWidget(MenuBar);
}
//-----------------------------------------------------------------------------
void ISMainWindow::CreateInformationMessage()
{
	QString InformationMessage = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_OTHER_INFORMATIONMESSAGE);
	if (InformationMessage.length())
	{
		QLabel *Label = new QLabel(this);
		Label->setFont(FONT_APPLICATION_BOLD);
		Label->setText(InformationMessage);
		Label->setWordWrap(true);
		Label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
		GetMainLayout()->addWidget(Label, 0, Qt::AlignCenter);

		GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

		QColor ColorMessage = SETTING_DATABASE_VALUE_COLOR(CONST_UID_DATABASE_SETTING_OTHER_INFORMATIONMESSAGE);

		QPalette Palette = Label->palette();
		Palette.setColor(QPalette::WindowText, ColorMessage);
		Label->setPalette(Palette);
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::CreateStackWidget()
{
	StackedWidget = new QStackedWidget(this);
	GetMainLayout()->addWidget(StackedWidget);

	for (int i = 0; i < ISParagraphEntity::GetInstance().GetParagraphs().count(); i++)
	{
		ISMetaParagraph *MetaParagraph = ISParagraphEntity::GetInstance().GetParagraphs().at(i);
		ISUuid ParagraphUID = MetaParagraph->GetUID();

		ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.Initialize.OpeningMainWindow.CreateParagparh").arg(MetaParagraph->GetLocalName()));
		ISCountingTime CountingTime;

		int ObjectType = QMetaType::type((MetaParagraph->GetClassName() + "*").toLocal8Bit().constData());
		IS_ASSERT(ObjectType, QString("Invalid object type from paragraph: %1").arg(MetaParagraph->GetName()));

		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		IS_ASSERT(MetaObject, QString("Invalid meta object from paragraph: %1").arg(MetaParagraph->GetName()));

		ISParagraphBaseForm *ParagraphBaseForm = dynamic_cast<ISParagraphBaseForm*>(MetaObject->newInstance(Q_ARG(QWidget *, this)));
		IS_ASSERT(ParagraphBaseForm, QString("Invalid class object from paragraph: %1").arg(MetaParagraph->GetName()));
		ParagraphBaseForm->SetButtonParagraph(MenuBar->GetParagraphButton(ParagraphUID));
		int ParagraphIndex = StackedWidget->addWidget(ParagraphBaseForm);
		Paragraphs.insert(ParagraphUID, ParagraphIndex);

		ISDebug::ShowDebugString(QString("Initialized paragraph \"%1\" %2 msec").arg(MetaParagraph->GetName()).arg(CountingTime.GetElapsed()));
	}

	MenuBar->ButtonParagraphClicked(ISParagraphEntity::GetInstance().GetDefaultParagraph());
}
//-----------------------------------------------------------------------------
void ISMainWindow::CreateStatusBar()
{
	if (SETTING_BOOL(CONST_UID_SETTING_STATUS_BAR_SHOWSTATUSBAR))
	{
		QFrame *Frame = ISControls::CreateHorizontalLine(this);
		GetMainLayout()->addWidget(Frame);

		QPalette Palette = Frame->palette();
		Palette.setColor(QPalette::Dark, COLOR_MAIN_MENU_BAR);
		Frame->setPalette(Palette);

		StatusBar = new ISStatusBar(this);
		GetMainLayout()->addWidget(StatusBar);
		connect(StatusBar, &ISStatusBar::DateTimeClicked, [=]
		{
			MenuBar->ButtonParagraphClicked(CONST_UID_PARAGRAPH_CALENDAR);
		});
		connect(StatusBar, &ISStatusBar::MakeCall, this, &ISMainWindow::MakeCall);
		connect(StatusBar, &ISStatusBar::TrayClicked, this, &ISMainWindow::TrayClicked);
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowCreateRecords()
{
	ISFastCreateRecordsForm FastCreateRecordsForm;
	FastCreateRecordsForm.Exec();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowExternalTools()
{
	ISExternalToolsForm FastAccessSettingsForm;
	FastAccessSettingsForm.Exec();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ParagraphClicked(const ISUuid &ParagraphUID)
{
	if (GetCurrentParagraphUID() == ParagraphUID)
	{
		return;
	}

	ISSystem::SetWaitGlobalCursor(true);

	int ParagraphIndex = Paragraphs.value(ParagraphUID);
	ISParagraphBaseForm *ParagraphWidget = dynamic_cast<ISParagraphBaseForm*>(StackedWidget->widget(ParagraphIndex));
	StackedWidget->setCurrentWidget(ParagraphWidget);
	ParagraphWidget->Invoke();
	
	ParagraphWidget->setFocus();
	SetCurrentParagraphUID(ParagraphUID);

	ISProtocol::Insert(true, CONST_UID_PROTOCOL_OPEN_PARAGRAPH, QString(), QString(), QVariant(), ISParagraphEntity::GetInstance().GetParagraph(ParagraphUID)->GetLocalName());

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::TerminateMe(const QVariantMap &VariantMap)
{
	activateWindow();
	raise();
	ISSystem::ProcessEvents();
	ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.TerminateMe"));
	BeforeClose();
	ISCore::ExitApplication();
}
//-----------------------------------------------------------------------------
void ISMainWindow::UpdateAviable(const QVariantMap &VariantMap)
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.NotifyUpdateAviable")))
	{
		if (ISCreatedObjectsEntity::GetInstance().CheckExistForms())
		{
			ISCore::ChangeUser();
		}
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::IncomingCall(const QVariantMap &VariantMap)
{
	ISSystem::SetWaitGlobalCursor(true);
	QString ClassName = ISLicense::GetInstance().GetIncomingCallForm();
	int ObjectType = QMetaType::type((ClassName + "*").toLocal8Bit().constData());
	const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
	ISIncomingCallBaseForm *IncomingCallForm = dynamic_cast<ISIncomingCallBaseForm*>(MetaObject->newInstance(Q_ARG(const QVariantMap &, VariantMap)));
	ISSystem::SetWaitGlobalCursor(false);
	IncomingCallForm->ExecAnimated();
}
//-----------------------------------------------------------------------------
void ISMainWindow::AlreadyConnected(const QVariantMap &VariantMap)
{
	ISMessageBox::ShowCritical(this, LOCALIZATION("Message.Warning.AlreadyConnected.MessageBox").arg(VariantMap.value("Payload").toString()));
}
//-----------------------------------------------------------------------------
void ISMainWindow::BeforeClose()
{
	hide();

	ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_USER_OFFLINE, QVariant(), CURRENT_USER_FULL_NAME, false);

	ISSplashScreen::GetInstance().DefaultPixmap();
	ISSplashScreen::GetInstance().show();
	ISSystem::ProcessEvents();

	ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.CloseApplication.FixingExitToProtocol"));
	ISProtocol::ExitApplication();
	ISOnline::GetInstance().Exit();

	ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.CloseApplication.SaveUserSortings"));
	ISSortingBuffer::GetInstance().SaveSortings();

	ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.CloseApplication.CommandOnExit"));
	ISCore::ExecuteExitComamnd();

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERCOLUMNSIZE))
	{
		ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.CloseApplication.ColumnSize.Save"));
		ISColumnSizer::GetInstance().Save();
	}
	else
	{
		ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.CloseApplication.ColumnSize.Clear"));
		ISColumnSizer::GetInstance().Clear();
	}

	ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.CloseApplication.DisconnectFromDatabase"));
	ISDatabase::GetInstance().DisconnectFromDefaultDB();

	ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.CloseApplication.ExitApplication"));
}
//-----------------------------------------------------------------------------
void ISMainWindow::OpenHistoryObject(const QString &TableName, int ObjectID)
{
	ISGui::CreateObjectForm(ISNamespace::ObjectFormType::OFT_Edit, TableName, ObjectID)->show();
}
//-----------------------------------------------------------------------------
void ISMainWindow::OpenFavoritesObject(const QString &TableName, int ObjectID)
{
	ISGui::CreateObjectForm(ISNamespace::ObjectFormType::OFT_Edit, TableName, ObjectID)->show();
}
//-----------------------------------------------------------------------------
void ISMainWindow::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISMainWindow::InitializePlugin()
{
	if (!SETTING_DATABASE_VALUE(CONST_UID_DATABASE_SETTING_GENERAL_ADMINISTRATOR).isValid()) //Если не настроен администратор - вывести предупреждение
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotSettingAdministratorInSettingDatabase"));
	}

	ISObjects::GetInstance().GetInterface()->InitializePlugin();
}
//-----------------------------------------------------------------------------
void ISMainWindow::LockClicked()
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.Lock")))
	{
		LockApplication();
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::ChangeUser()
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.ChangeUser")))
	{
		BeforeClose();
		ISCore::ChangeUser();
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::ActivateWorkspace()
{
	ParagraphClicked(CONST_UID_PARAGRAPH_WORKSPACE);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowFavoritesForm()
{
	ISSystem::SetWaitGlobalCursor(true);

	ISFavoritesForm *FavoritesForm = new ISFavoritesForm();
	connect(FavoritesForm, &ISFavoritesForm::OpenObject, this, &ISMainWindow::OpenFavoritesObject);
	FavoritesForm->show();

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowHistoryForm()
{
	ISSystem::SetWaitGlobalCursor(true);

	ISHistoryForm *HistoryForm = new ISHistoryForm();
	connect(HistoryForm, &ISHistoryForm::OpenObject, this, &ISMainWindow::OpenHistoryObject);
	HistoryForm->show();

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowDeviceSettings()
{
	ISDeviceSettingsForm *DeviceSettingsForm = new ISDeviceSettingsForm();
	DeviceSettingsForm->show();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowChangePasswordForm()
{
	if (CURRENT_USER_SYSTEM)
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotChangeSystemUserPassword"));
		return;
	}

	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_CHANGE_PASSWORD))
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotAccess.Special.UserPasswordChange"));
		return;
	}

	if (ISGui::ShowUserPasswordForm(CURRENT_USER_ID))
	{
		ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.YouPasswordDoneChanged"));
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::CreateLogToday()
{
	QString LogPath = ISLogger::GetInstance().GetLogFileName();
	QString FileName = ISSystem::GetFileName(LogPath);
	QString FilePath = ISFileDialog::GetSaveFileName(this, LOCALIZATION("File.Filter.Log"), FileName);
	if (FilePath.length())
	{
		QFile FileLog(LogPath);
		if (!FileLog.copy(FilePath))
		{
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotCreateLogFile"), FileLog.errorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowNoteForm()
{
	ISSystem::SetWaitGlobalCursor(true);

	ISNoteForm *NoteListForm = new ISNoteForm();
	NoteListForm->show();

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowCalculator()
{
	ISCalculatorForm *CalculatorForm = new ISCalculatorForm();
	CalculatorForm->show();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAddressBook()
{
	ISAddressBookListForm *AddressBookListForm = new ISAddressBookListForm();
	AddressBookListForm->LoadData();
	AddressBookListForm->showMaximized();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowDebugApplication()
{
	ISSystem::SetWaitGlobalCursor(true);
	ISSystem::OpenFile(DEBUG_VIEW_PATH);
	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::UserStatusChange()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_USER_STATUS_CHANGE))
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotAccess.Special.UserStatusChange"));
		return;
	}

	SetVisibleShadow(true);
	raise();
	activateWindow();

	ISSystem::SetWaitGlobalCursor(true);
	ISUserStatusForm UserStatusForm;
	ISSystem::SetWaitGlobalCursor(false);
	if (UserStatusForm.Exec())
	{
		QString StatusName = ISMetaUser::GetInstance().GetCurrentStatus(CURRENT_USER_ID);
		ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_USER_STATUS, QVariant(), CURRENT_USER_FULL_NAME + ": " + StatusName, false);
	}

	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowSettingsForm()
{
	if (SETTING_BOOL(CONST_UID_SETTING_SECURITY_PASSWORDSETTINGSSHOW))
	{
		QString InputPassword = ISInputDialog::GetPassword(this, LOCALIZATION("Security"), LOCALIZATION("EnterThePasswordForShowSettingsForm")).toString();
		if (InputPassword.length())
		{
			if (InputPassword != SETTING_STRING(CONST_UID_SETTING_SECURITY_PASSWORDSETTINGS))
			{
				ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Password.Error"));
				return;
			}
		}
	}

	ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_GENERAL);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowHelpSystem()
{
	
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAboutForm()
{
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_SHOW_ABOUT_FORM, QString(), QString(), QVariant());
	ISAboutForm AboutForm;
	AboutForm.Exec();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowFeedbackForm()
{
	ISSystem::SetWaitGlobalCursor(true);

	ISFeedbackForm *FeedbackForm = new ISFeedbackForm();
	FeedbackForm->show();

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAboutQt()
{
	QApplication::aboutQt();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowLicenseForm()
{
	ISLicenseForm LicenseForm;
	LicenseForm.Exec();
}
//-----------------------------------------------------------------------------
void ISMainWindow::MakeCall()
{
	if (ISTelephony::CheckSetUp())
	{
		ISSystem::SetWaitGlobalCursor(true);

		PhoneForm->adjustSize();

		QPoint Point = StatusBar->mapToGlobal(QPoint());
		Point.setX(Point.x() + StatusBar->width() - PhoneForm->width() - 25);
		Point.setY(Point.y() - PhoneForm->height() - 10);

		PhoneForm->move(Point);
		PhoneForm->ShowAnimated(false, 400);

		ISSystem::SetWaitGlobalCursor(false);
	}
	else
	{
		ISMessageBox::ShowInformation(this, LOCALIZATION("NotSettingTelephonyForCurrentUser"));
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::TrayClicked()
{
	hide();

	SystemTrayIcon->show();
	SystemTrayIcon->showMessage(LOCALIZATION("Information"), LOCALIZATION("TrayMessage"), QSystemTrayIcon::Information, 4000);
}
//-----------------------------------------------------------------------------
void ISMainWindow::SystemTrayActivated(QSystemTrayIcon::ActivationReason Reason)
{
	if (SETTING_BOOL(CONST_UID_SETTING_VIEW_STARTMAINWINDOWANIMATED))
	{
		ShowAnimated();
	}
	else
	{
		show();
	}

	SystemTrayIcon->hide();
}
//-----------------------------------------------------------------------------
