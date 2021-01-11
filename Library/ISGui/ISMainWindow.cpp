#include "ISMainWindow.h"
#include "ISDefinesGui.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISAboutDialog.h"
#include "ISControls.h"
#include "ISUserRoleEntity.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISParagraphEntity.h"
#include "ISPopupMessage.h"
#include "ISCreatedObjectsEntity.h"
#include "ISObjects.h"
#include "ISAlgorithm.h"
#include "ISProperty.h"
#include "ISVersionInfo.h"
#include "ISConfig.h"
#include "ISTcpConnector.h"
#include "ISReconnectDialog.h"
#include "ISPopupMessage.h"
#include "ISTcpQuery.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISMainWindow::ISMainWindow(QWidget *parent)
	: ISInterfaceForm(parent),
	ExitConfirm(true),
	ExistCheckModifie(true),
	PropertyAnimation(nullptr)
{
	connect(&ISCreatedObjectsEntity::Instance(), &ISCreatedObjectsEntity::Existed, this, &ISMainWindow::ActivateWorkspace);

	ISVersionInfo::Instance().ConfigurationInfo.LocalName.isEmpty() ?
		setWindowTitle("IntegralSystem: " + ISBuffer::Instance().CurrentUserInfo.FIO) :
		setWindowTitle("IntegralSystem - " + ISVersionInfo::Instance().ConfigurationInfo.LocalName + " : " + ISBuffer::Instance().CurrentUserInfo.FIO);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowIcon(BUFFER_ICONS("Logo"));
	resize(ISDefines::Gui::SIZE_MAIN_WINDOW);
	setMinimumSize(ISDefines::Gui::SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setSpacing(0);

	MenuBar = new ISMenuBar(this);
	connect(MenuBar, static_cast<void(ISMenuBar::*)(const ISUuid &)>(&ISMenuBar::ParagraphClicked), this, &ISMainWindow::ParagraphClicked);
	connect(MenuBar, &ISMenuBar::ChangeUser, this, &ISMainWindow::ChangeUser);
	connect(MenuBar, &ISMenuBar::Exit, this, &ISMainWindow::close);
	connect(MenuBar, &ISMenuBar::Favorites, this, &ISMainWindow::ShowFavoritesForm);
	connect(MenuBar, &ISMenuBar::History, this, &ISMainWindow::ShowHistoryForm);
	connect(MenuBar, &ISMenuBar::DatabaseSettings, this, &ISMainWindow::ShowDatabaseSettings);
	connect(MenuBar, &ISMenuBar::ChangePassword, this, &ISMainWindow::ShowChangePasswordForm);
	connect(MenuBar, &ISMenuBar::Settings, this, &ISMainWindow::ShowSettingsForm);
	connect(MenuBar, &ISMenuBar::AboutApplication, this, &ISMainWindow::ShowAboutForm);
	GetMainLayout()->addWidget(MenuBar);

	StackedWidget = new QStackedWidget(this);
	GetMainLayout()->addWidget(StackedWidget);
	for (ISMetaParagraph *MetaParagraph : ISParagraphEntity::Instance().GetParagraphs())
	{
		Paragraphs[MetaParagraph->UID] = StackedWidget->addWidget(ISAlgorithm::CreatePointer<ISParagraphBaseForm *>(MetaParagraph->ClassName, Q_ARG(QWidget *, this)));
	}

	if (CONFIG_BOOL("Protocol/Include"))
	{
		connect(&ISTcpConnector::Instance(), &ISTcpConnector::Reconnect, this, &ISMainWindow::Reconnect);
	}
}
//-----------------------------------------------------------------------------
ISMainWindow::~ISMainWindow()
{

}
//-----------------------------------------------------------------------------
void ISMainWindow::closeEvent(QCloseEvent *CloseEvent)
{
	if (ExistCheckModifie) //ѕровер€ть наличие несохраненных записей
	{
		if (!ISCreatedObjectsEntity::Instance().CheckExistForms())
		{
			CloseEvent->ignore();
		}
	}
	
	bool Answer = true;
	if (ExitConfirm) //“ребуетс€ подтверждение выхода
	{
		SetVisibleShadow(true);
		Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ExitApplication"));
		SetVisibleShadow(false);
	}
	//ѕри подтверждении выхода об€зательно вызывать quit(), иначе основной поток событий будет висеть и программа не закроетс€
	Answer ? qApp->quit() : CloseEvent->ignore();
}
//-----------------------------------------------------------------------------
void ISMainWindow::AfterShowEvent()
{
	ISInterfaceForm::AfterShowEvent();
	QTimer::singleShot(3000, ISObjects::Instance().GetInterface(), &ISObjectInterface::InitializePlugin);
}
//-----------------------------------------------------------------------------
void ISMainWindow::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ParagraphClicked(const ISUuid &ParagraphUID)
{
	if (CurrentParagraphUID == ParagraphUID)
	{
		return;
	}
	ISGui::SetWaitGlobalCursor(true);
	ISParagraphBaseForm *ParagraphWidget = dynamic_cast<ISParagraphBaseForm*>(StackedWidget->widget(Paragraphs[ParagraphUID]));
	StackedWidget->setCurrentWidget(ParagraphWidget);
	ParagraphWidget->Invoke();
	ParagraphWidget->setFocus();
	CurrentParagraphUID = ParagraphUID;
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::RollUp()
{
	PropertyAnimation->start();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ChangeUser()
{
	ExitConfirm = false;
	SetVisibleShadow(true);
	bool Change = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ChangeUser"));
	SetVisibleShadow(false);
	if (Change)
	{
		ExitConfirm = false;
		PROPERTY_SET("is_change_user", true);
		close();
	}
	else
	{
		ExitConfirm = true;
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
	ISGui::ShowFavoritesForm();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowHistoryForm()
{
	ISGui::ShowHistoryForm();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowDatabaseSettings()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_SettingsDatabase", 5));
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowChangePasswordForm()
{
	if (ISBuffer::Instance().CurrentUserInfo.System) //≈сли пользователь системный - не разрешаем мен€ть пароль
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotChangeSystemUserPassword"));
		return;
	}
	SetVisibleShadow(true);
	ISGui::ShowUserPasswordForm(CURRENT_USER_ID, ISBuffer::Instance().CurrentUserInfo.FIO, ISBuffer::Instance().CurrentUserInfo.Login);
	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowSettingsForm()
{
	SetVisibleShadow(true);
	ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_GENERAL);
	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAboutForm()
{
	SetVisibleShadow(true);
	ISAboutDialog().Exec();
	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::Reconnect()
{
	SetVisibleShadow(true);
	ISReconnectDialog ReconnectForm;
	bool Result = ReconnectForm.Exec();
	if (Result) //ѕереподключение прошло успешно - посылаем запрос на авторизацию
	{
		ISTcpQuery qReAuth(API_AUTH);
		qReAuth.BindValue("Hash", ISSystem::StringToSha256(ISBuffer::Instance().CurrentUserInfo.Login + ISBuffer::Instance().CurrentUserInfo.Password));
		Result = qReAuth.Execute();
		if (Result)
		{
			ISPopupMessage::ShowNotification(LANG("ReconnectingDone"));

			//—ервер веро€тнее всего обновилс€ - предупреждаем об этом
			if (qReAuth.GetAnswer()["IsNeedUpdate"].toBool())
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.NewVersion"));
			}
		}
		else
		{
			ISMessageBox::ShowCritical(this, qReAuth.GetErrorString());
		}
	}
	
	if (!Result) //ѕереподключени€ не произошло - выходим из программы
	{
		ExitConfirm = false;
		ExistCheckModifie = false;
		close();
	}
	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
