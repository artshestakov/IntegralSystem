#include "ISMainWindow.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISProtocol.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISMetaUser.h"
#include "ISSettings.h"
#include "ISFavoritesForm.h"
#include "ISAboutForm.h"
#include "ISSettingsDatabase.h"
#include "ISControls.h"
#include "ISNoteForm.h"
#include "ISUserRoleEntity.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISHistoryForm.h"
#include "ISFastCreateObjectForm.h"
#include "ISExternalToolsForm.h"
#include "ISParagraphEntity.h"
#include "ISNotifyRecipient.h"
#include "ISNotifySender.h"
#include "ISNotificationService.h"
#include "ISCreatedObjectsEntity.h"
#include "ISIncomingCallBaseForm.h"
#include "ISDeviceSettingsForm.h"
#include "ISTelephony.h"
#include "ISObjects.h"
#include "ISAddressBookListForm.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISMainWindow::ISMainWindow(QWidget *parent) : ISInterfaceForm(parent)
{
	connect(&ISCreatedObjectsEntity::Instance(), &ISCreatedObjectsEntity::Existed, this, &ISMainWindow::ActivateWorkspace);

	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowIcon(BUFFER_PIXMAPS("Logo"));
	setWindowTitle(QString("IntegralSystem - %1 : %2").arg(ISObjects::GetInstance().GetInfo().LocalName).arg(ISMetaUser::Instance().UserData->FullName));
	resize(ISDefines::Gui::SIZE_MAIN_WINDOW);
	setMinimumSize(ISDefines::Gui::SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setSpacing(0);

	CreateMenuBar();
	CreateInformationMessage();
	CreateStackWidget();
	CreateStatusBar();

	//�������� ������
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::IncomingCall, this, &ISMainWindow::IncomingCall);
}
//-----------------------------------------------------------------------------
ISMainWindow::~ISMainWindow()
{

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
void ISMainWindow::closeEvent(QCloseEvent *CloseEvent)
{
	if (ISCreatedObjectsEntity::Instance().CheckExistForms())
	{
		if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_CONFIRMEXITAPPLICATION))
		{
			SetVisibleShadow(true);
			bool Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ExitApplication"));
			SetVisibleShadow(false);
			Answer ? CloseEvent->accept() : CloseEvent->ignore();
		}
		else
		{
			ISGui::ExitApplication();
		}
	}
	else
	{
		CloseEvent->ignore();
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
	connect(MenuBar, &ISMenuBar::ChangeUser, this, &ISMainWindow::ChangeUser);
	connect(MenuBar, &ISMenuBar::Exit, this, &ISMainWindow::close);
	connect(MenuBar, &ISMenuBar::Favorites, this, &ISMainWindow::ShowFavoritesForm);
	connect(MenuBar, &ISMenuBar::History, this, &ISMainWindow::ShowHistoryForm);
	connect(MenuBar, &ISMenuBar::DeviceSettings, this, &ISMainWindow::ShowDeviceSettings);
	connect(MenuBar, &ISMenuBar::ChangePassword, this, &ISMainWindow::ShowChangePasswordForm);
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
	if (!InformationMessage.isEmpty())
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
	for (ISMetaParagraph *MetaParagraph : ISParagraphEntity::GetInstance().GetParagraphs())
	{
		ISParagraphBaseForm *ParagraphBaseForm = ISAlgorithm::CreatePointer<ISParagraphBaseForm *>(MetaParagraph->ClassName, Q_ARG(QWidget *, this));
		ParagraphBaseForm->SetButtonParagraph(MenuBar->GetParagraphButton(MetaParagraph->UID));
		Paragraphs.emplace(MetaParagraph->UID, StackedWidget->addWidget(ParagraphBaseForm));
	}
	MenuBar->ButtonParagraphClicked(ISParagraphEntity::GetInstance().GetDefaultParagraph());
}
//-----------------------------------------------------------------------------
void ISMainWindow::CreateStatusBar()
{
	if (SETTING_BOOL(CONST_UID_SETTING_STATUS_BAR_SHOWSTATUSBAR))
	{
		StatusBar = new ISStatusBar(this);
		StatusBar->setVisible(false);
		GetMainLayout()->addWidget(StatusBar);
	}
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowCreateRecords()
{
	ISFastCreateRecordsForm().Exec();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowExternalTools()
{
	ISExternalToolsForm().Exec();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ParagraphClicked(const ISUuid &ParagraphUID)
{
	if (GetCurrentParagraphUID() == ParagraphUID)
	{
		return;
	}

	ISGui::SetWaitGlobalCursor(true);

	ISParagraphBaseForm *ParagraphWidget = dynamic_cast<ISParagraphBaseForm*>(StackedWidget->widget(Paragraphs[ParagraphUID]));
	StackedWidget->setCurrentWidget(ParagraphWidget);
	ParagraphWidget->Invoke();
	
	ParagraphWidget->setFocus();
	SetCurrentParagraphUID(ParagraphUID);

	ISProtocol::Insert(true, CONST_UID_PROTOCOL_OPEN_PARAGRAPH, QString(), QString(), QVariant(), ISParagraphEntity::GetInstance().GetParagraph(ParagraphUID)->LocalName);
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::IncomingCall(const QVariantMap &VariantMap)
{
	ISGui::SetWaitGlobalCursor(true);
	ISIncomingCallBaseForm *IncomingCallForm = ISAlgorithm::CreatePointer<ISIncomingCallBaseForm *>(ISObjects::GetInstance().GetInfo().IncomingCallForm, Q_ARG(const QVariantMap &, VariantMap));
	ISGui::SetWaitGlobalCursor(false);
	IncomingCallForm->ExecAnimated();
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
	//���� �� �������� ������������� - ���������� ���������
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
void ISMainWindow::ChangeUser()
{
	SetVisibleShadow(true);
	bool Change = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ChangeUser"));
	SetVisibleShadow(false);
	if (Change)
	{
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

	if (ISGui::ShowUserPasswordForm(CURRENT_USER_ID))
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
void ISMainWindow::ShowSettingsForm()
{
	ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_GENERAL);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAboutForm()
{
	SetVisibleShadow(true);
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_SHOW_ABOUT_FORM, QString(), QString(), QVariant());
	ISAboutForm().Exec();
	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAboutQt()
{
	QApplication::aboutQt();
}
//-----------------------------------------------------------------------------