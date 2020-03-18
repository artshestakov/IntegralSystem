#include "ISMainWindow.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
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
#include "ISLockForm.h"
#include "ISExitForm.h"
#include "ISMemoryObjects.h"
#include "ISColumnSizer.h"
#include "ISHistoryForm.h"
#include "ISFastCreateObjectForm.h"
#include "ISExternalToolsForm.h"
#include "ISInputDialog.h"
#include "ISParagraphEntity.h"
#include "ISNotifyRecipient.h"
#include "ISNotifySender.h"
#include "ISNotificationService.h"
#include "ISCreatedObjectsEntity.h"
#include "ISLogger.h"
#include "ISIncomingCallBaseForm.h"
#include "ISUserStatusForm.h"
#include "ISDeviceSettingsForm.h"
#include "ISTelephony.h"
#include "ISObjects.h"
#include "ISOnline.h"
#include "ISAddressBookListForm.h"
//-----------------------------------------------------------------------------
ISMainWindow::ISMainWindow(QWidget *parent)
	: ISInterfaceForm(parent),
	CloseEvent(true)
{
	ISMemoryObjects::GetInstance().SetMainWindow(this);
	connect(&ISCreatedObjectsEntity::GetInstance(), &ISCreatedObjectsEntity::Existed, this, &ISMainWindow::ActivateWorkspace);

	setWindowIcon(BUFFER_PIXMAPS("Logo"));
	setWindowTitle(QString("IntegralSystem - %1 : %2").arg(ISObjects::GetInstance().GetInfo().LocalName).arg(ISMetaUser::Instance().UserData->FullName));
	resize(ISDefines::Gui::SIZE_MAIN_WINDOW);
	setMinimumSize(ISDefines::Gui::SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setSpacing(0);

	CreateMenuBar();
	CreateInformationMessage();
	CreateStackWidget();
	CreateStatusBar();

	PhoneForm = new ISPhoneForm();

	//Отключение пользователя
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::TermianteUser, this, &ISMainWindow::TerminateMe);

	//Доступно обновление
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::UpdateAviable, this, &ISMainWindow::UpdateAviable);

	//Входящий звонок
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::IncomingCall, this, &ISMainWindow::IncomingCall);
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
				QString Password = ISInputDialog::GetPassword(this, LANG("Security"), LANG("EnterThePasswordForExit")).toString();
				if (Password.length())
				{
					if (Password != SETTING_STRING(CONST_UID_SETTING_SECURITY_PASSWORDEXIT))
					{
						e->ignore();
						ISMessageBox::ShowWarning(this, LANG("Message.Password.Error"));
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

				ISGui::SetWaitGlobalCursor(true);
				ISExitForm ExitForm;
				ExitForm.raise();
				connect(&ExitForm, &ISExitForm::Showed, [=] { ISGui::SetWaitGlobalCursor(false); });

				if (ExitForm.Exec())
				{
					switch (ExitForm.GetSelectedAction())
					{
					case ISNamespace::EFA_Lock:
						e->ignore();
						LockApplication();
						break;

					case ISNamespace::EFA_ChangeUser:
						e->ignore();
						BeforeClose();
						ISGui::ChangeUser();
						break;

					case ISNamespace::EFA_Exit:
						BeforeClose();
						ISGui::ExitApplication();
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
				ISGui::ExitApplication();
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
		ISNotificationService::ShowNotification(LANG("NewNotifications").arg(NewNotifyCount));
		QApplication::beep();
	}

	if (ISMetaUser::Instance().UserData->FixedInactive)
	{
		ISOnline::GetInstance().Initialize(ISMetaUser::Instance().UserData->InactiveTimeout);
	}

	ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_USER_ONLINE, QVariant(), ISMetaUser::Instance().UserData->FullName, false);

	int CountOverdue = ISCore::TaskCountOverdue();
	if (CountOverdue)
	{
		ISNotificationService::ShowNotification(LANG("YouHaveExpiredTasks").arg(CountOverdue));
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
	connect(MenuBar, &ISMenuBar::UserStatus, this, &ISMainWindow::UserStatusChange);
	connect(MenuBar, &ISMenuBar::Settings, this, &ISMainWindow::ShowSettingsForm);
	connect(MenuBar, &ISMenuBar::Notebook, this, &ISMainWindow::ShowNoteForm);
	connect(MenuBar, &ISMenuBar::AddressBook, this, &ISMainWindow::ShowAddressBook);
	connect(MenuBar, &ISMenuBar::AboutApplication, this, &ISMainWindow::ShowAboutForm);
	connect(MenuBar, &ISMenuBar::AboutQt, this, &ISMainWindow::ShowAboutQt);
	GetMainLayout()->addWidget(MenuBar);
}
//-----------------------------------------------------------------------------
void ISMainWindow::CreateInformationMessage()
{
	QString InformationMessage = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_OTHER_INFORMATIONMESSAGE);
	if (InformationMessage.length())
	{
		QLabel *Label = new QLabel(this);
		Label->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		Label->setText(InformationMessage);
		Label->setWordWrap(true);
		Label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
		GetMainLayout()->addWidget(Label, 0, Qt::AlignCenter);

		GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

		QPalette Palette = Label->palette();
		Palette.setColor(QPalette::WindowText, ISGui::StringToColor(SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_OTHER_INFORMATIONMESSAGE)));
		Label->setPalette(Palette);
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::CreateStackWidget()
{
	StackedWidget = new QStackedWidget(this);
	GetMainLayout()->addWidget(StackedWidget);

	for (int i = 0; i < ISParagraphEntity::GetInstance().GetParagraphs().count(); ++i)
	{
		ISMetaParagraph *MetaParagraph = ISParagraphEntity::GetInstance().GetParagraphs().at(i);
		ISUuid ParagraphUID = MetaParagraph->UID;

		ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.OpeningMainWindow.CreateParagparh").arg(MetaParagraph->LocalName));

		int ObjectType = QMetaType::type((MetaParagraph->ClassName + SYMBOL_STAR).toLocal8Bit().constData());
		IS_ASSERT(ObjectType, QString("Invalid object type from paragraph: %1").arg(MetaParagraph->Name));

		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		IS_ASSERT(MetaObject, QString("Invalid meta object from paragraph: %1").arg(MetaParagraph->Name));

		ISParagraphBaseForm *ParagraphBaseForm = dynamic_cast<ISParagraphBaseForm*>(MetaObject->newInstance(Q_ARG(QWidget *, this)));
		IS_ASSERT(ParagraphBaseForm, QString("Invalid class object from paragraph: %1").arg(MetaParagraph->Name));
		ParagraphBaseForm->SetButtonParagraph(MenuBar->GetParagraphButton(ParagraphUID));
		int ParagraphIndex = StackedWidget->addWidget(ParagraphBaseForm);
		Paragraphs.insert(ParagraphUID, ParagraphIndex);

		ISLOGGER_DEBUG(QString("Initialized paragraph \"%1\"").arg(MetaParagraph->Name));
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
		Palette.setColor(QPalette::Dark, ISDefines::Gui::COLOR_MAIN_MENU_BAR);
		Frame->setPalette(Palette);

		StatusBar = new ISStatusBar(this);
		GetMainLayout()->addWidget(StatusBar);
		connect(StatusBar, &ISStatusBar::DateTimeClicked, [=]
		{
			MenuBar->ButtonParagraphClicked(CONST_UID_PARAGRAPH_CALENDAR);
		});
		connect(StatusBar, &ISStatusBar::MakeCall, this, &ISMainWindow::MakeCall);
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

	ISGui::SetWaitGlobalCursor(true);

	int ParagraphIndex = Paragraphs.value(ParagraphUID);
	ISParagraphBaseForm *ParagraphWidget = dynamic_cast<ISParagraphBaseForm*>(StackedWidget->widget(ParagraphIndex));
	StackedWidget->setCurrentWidget(ParagraphWidget);
	ParagraphWidget->Invoke();
	
	ParagraphWidget->setFocus();
	SetCurrentParagraphUID(ParagraphUID);

	ISProtocol::Insert(true, CONST_UID_PROTOCOL_OPEN_PARAGRAPH, QString(), QString(), QVariant(), ISParagraphEntity::GetInstance().GetParagraph(ParagraphUID)->LocalName);
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::TerminateMe(const QVariantMap &VariantMap)
{
	activateWindow();
	raise();
	ISGui::ProcessEvents();
	ISMessageBox::ShowInformation(this, LANG("Message.Information.TerminateMe"));
	BeforeClose();
	ISGui::ExitApplication();
}
//-----------------------------------------------------------------------------
void ISMainWindow::UpdateAviable(const QVariantMap &VariantMap)
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.NotifyUpdateAviable")))
	{
		if (ISCreatedObjectsEntity::GetInstance().CheckExistForms())
		{
			ISGui::ChangeUser();
		}
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::IncomingCall(const QVariantMap &VariantMap)
{
	ISGui::SetWaitGlobalCursor(true);
	ISIncomingCallBaseForm *IncomingCallForm = nullptr;
	int ObjectType = QMetaType::type((ISObjects::GetInstance().GetInfo().IncomingCallForm + SYMBOL_STAR).toLocal8Bit().constData());
	if (ObjectType)
	{
		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		if (MetaObject)
		{
			IncomingCallForm = dynamic_cast<ISIncomingCallBaseForm*>(MetaObject->newInstance(Q_ARG(const QVariantMap &, VariantMap)));
		}
	}

	ISGui::SetWaitGlobalCursor(false);
	if (IncomingCallForm)
	{
		IncomingCallForm->ExecAnimated();
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::BeforeClose()
{
	hide();

	ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_USER_OFFLINE, QVariant(), ISMetaUser::Instance().UserData->FullName, false);

	ISSplashScreen::GetInstance().DefaultPixmap();
	ISSplashScreen::GetInstance().show();
	ISGui::ProcessEvents();

	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.CloseApplication.FixingExitToProtocol"));
	ISProtocol::ExitApplication();
	ISOnline::GetInstance().Exit();

	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.CloseApplication.SaveUserSortings"));
	ISSortingBuffer::GetInstance().SaveSortings();

	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.CloseApplication.CommandOnExit"));
	ISCore::ExecuteExitComamnd();

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_REMEMBERCOLUMNSIZE))
	{
		ISSplashScreen::GetInstance().SetMessage(LANG("Banner.CloseApplication.ColumnSize.Save"));
		ISColumnSizer::Instance().Save();
	}
	else
	{
		ISSplashScreen::GetInstance().SetMessage(LANG("Banner.CloseApplication.ColumnSize.Clear"));
		ISColumnSizer::Instance().Clear();
	}

	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.CloseApplication.DisconnectFromDatabase"));
	ISDatabase::Instance().Disconnect(CONNECTION_DEFAULT);

	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.CloseApplication.ExitApplication"));
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
	//Если не настроен администратор - предложить настроить
	if (!SETTING_DATABASE_VALUE(CONST_UID_DATABASE_SETTING_GENERAL_ADMINISTRATOR).isValid())
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.NotSettingAdministratorInSettingDatabase")))
		{
			ISGui::ShowDatabaseSettings();
		}
	}
	ISObjects::GetInstance().GetInterface()->InitializePlugin();
}
//-----------------------------------------------------------------------------
void ISMainWindow::LockClicked()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.Lock")))
	{
		LockApplication();
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::ChangeUser()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ChangeUser")))
	{
		BeforeClose();
		ISGui::ChangeUser();
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
	ISGui::SetWaitGlobalCursor(true);
	ISFavoritesForm *FavoritesForm = new ISFavoritesForm();
	connect(FavoritesForm, &ISFavoritesForm::OpenObject, this, &ISMainWindow::OpenFavoritesObject);
	FavoritesForm->show();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowHistoryForm()
{
	ISGui::SetWaitGlobalCursor(true);
	ISHistoryForm *HistoryForm = new ISHistoryForm();
	connect(HistoryForm, &ISHistoryForm::OpenObject, this, &ISMainWindow::OpenHistoryObject);
	HistoryForm->show();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowDeviceSettings()
{
	(new ISDeviceSettingsForm())->show();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowChangePasswordForm()
{
	if (ISMetaUser::Instance().UserData->System)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotChangeSystemUserPassword"));
		return;
	}

	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_CHANGE_PASSWORD))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.UserPasswordChange"));
		return;
	}

	if (ISGui::ShowUserPasswordForm(ISMetaUser::Instance().UserData->ID))
	{
		ISMessageBox::ShowInformation(this, LANG("Message.Information.YouPasswordDoneChanged"));
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowNoteForm()
{
	ISGui::SetWaitGlobalCursor(true);
	(new ISNoteForm())->show();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAddressBook()
{
	ISAddressBookListForm *AddressBookListForm = new ISAddressBookListForm();
	AddressBookListForm->LoadData();
	AddressBookListForm->showMaximized();
}
//-----------------------------------------------------------------------------
void ISMainWindow::UserStatusChange()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_USER_STATUS_CHANGE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.UserStatusChange"));
		return;
	}

	SetVisibleShadow(true);
	raise();
	activateWindow();

	ISGui::SetWaitGlobalCursor(true);
	ISUserStatusForm UserStatusForm;
	ISGui::SetWaitGlobalCursor(false);
	if (UserStatusForm.Exec())
	{
		QString StatusName = ISMetaUser::Instance().GetCurrentStatus(ISMetaUser::Instance().UserData->ID);
		ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_USER_STATUS, QVariant(), ISMetaUser::Instance().UserData->FullName + ": " + StatusName, false);
	}

	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowSettingsForm()
{
	if (SETTING_BOOL(CONST_UID_SETTING_SECURITY_PASSWORDSETTINGSSHOW))
	{
		QString InputPassword = ISInputDialog::GetPassword(this, LANG("Security"), LANG("EnterThePasswordForShowSettingsForm")).toString();
		if (InputPassword.length())
		{
			if (InputPassword != SETTING_STRING(CONST_UID_SETTING_SECURITY_PASSWORDSETTINGS))
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Password.Error"));
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
void ISMainWindow::ShowAboutQt()
{
	QApplication::aboutQt();
}
//-----------------------------------------------------------------------------
void ISMainWindow::MakeCall()
{
	if (ISTelephony::CheckSetUp())
	{
		ISGui::SetWaitGlobalCursor(true);

		PhoneForm->adjustSize();

		QPoint Point = StatusBar->mapToGlobal(QPoint());
		Point.setX(Point.x() + StatusBar->width() - PhoneForm->width() - 25);
		Point.setY(Point.y() - PhoneForm->height() - 10);

		PhoneForm->move(Point);
		PhoneForm->ShowAnimated(false, 400);

		ISGui::SetWaitGlobalCursor(false);
	}
	else
	{
		ISMessageBox::ShowInformation(this, LANG("NotSettingTelephonyForCurrentUser"));
	}
}
//-----------------------------------------------------------------------------
