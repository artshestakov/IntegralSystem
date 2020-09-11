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
#include "ISAboutForm.h"
#include "ISSettingsDatabase.h"
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
//-----------------------------------------------------------------------------
ISMainWindow::ISMainWindow(QWidget *parent)
	: ISInterfaceForm(parent),
	ExitConfirm(true),
	PropertyAnimation(nullptr)
{
	connect(&ISCreatedObjectsEntity::Instance(), &ISCreatedObjectsEntity::Existed, this, &ISMainWindow::ActivateWorkspace);

	ISObjects::Instance().Info.LocalName.isEmpty() ?
		setWindowTitle("IntegralSystem: " + ISMetaUser::Instance().UserData.FIO) :
		setWindowTitle("IntegralSystem - " + ISObjects::Instance().Info.LocalName + " : " + ISMetaUser::Instance().UserData.FIO);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowIcon(BUFFER_ICONS("Logo"));
	resize(ISDefines::Gui::SIZE_MAIN_WINDOW);
	setMinimumSize(ISDefines::Gui::SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setSpacing(0);

	MenuBar = new ISMenuBar(this);
	connect(MenuBar, static_cast<void(ISMenuBar::*)(const ISUuid &)>(&ISMenuBar::ParagraphClicked), this, &ISMainWindow::ParagraphClicked);
	connect(MenuBar, &ISMenuBar::ChangeUser, this, &ISMainWindow::ChangeUser);
	connect(MenuBar, &ISMenuBar::RollUp, this, &ISMainWindow::RollUp);
	connect(MenuBar, &ISMenuBar::Exit, this, &ISMainWindow::close);
	connect(MenuBar, &ISMenuBar::Favorites, this, &ISMainWindow::ShowFavoritesForm);
	connect(MenuBar, &ISMenuBar::History, this, &ISMainWindow::ShowHistoryForm);
	connect(MenuBar, &ISMenuBar::ChangePassword, this, &ISMainWindow::ShowChangePasswordForm);
	connect(MenuBar, &ISMenuBar::Settings, this, &ISMainWindow::ShowSettingsForm);
	connect(MenuBar, &ISMenuBar::AboutApplication, this, &ISMainWindow::ShowAboutForm);
	connect(MenuBar, &ISMenuBar::AboutQt, this, &ISMainWindow::ShowAboutQt);
	GetMainLayout()->addWidget(MenuBar);

	StackedWidget = new QStackedWidget(this);
	GetMainLayout()->addWidget(StackedWidget);
	for (ISMetaParagraph *MetaParagraph : ISParagraphEntity::Instance().GetParagraphs())
	{
		Paragraphs[MetaParagraph->UID] = StackedWidget->addWidget(ISAlgorithm::CreatePointer<ISParagraphBaseForm *>(MetaParagraph->ClassName, Q_ARG(QWidget *, this)));
	}
	MenuBar->ParagraphClick(ISParagraphEntity::Instance().GetStartedParagraph());
}
//-----------------------------------------------------------------------------
ISMainWindow::~ISMainWindow()
{

}
//-----------------------------------------------------------------------------
void ISMainWindow::closeEvent(QCloseEvent *CloseEvent)
{
	if (ISCreatedObjectsEntity::Instance().CheckExistForms())
	{
		bool Answer = true;
		if (ExitConfirm)
		{
			SetVisibleShadow(true);
			Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ExitApplication"));
			SetVisibleShadow(false);
		}
		//ѕри подтверждении выхода об€зательно вызывать quit(), иначе основной поток событий будет висеть и программа не закроетс€
		Answer ? qApp->quit() : CloseEvent->ignore();
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
		PropertyAnimation = new QPropertyAnimation(this, "windowOpacity", this);
		PropertyAnimation->setStartValue(1.0);
		PropertyAnimation->setEndValue(0.0);
		PropertyAnimation->setDuration(100);
		connect(PropertyAnimation, &QPropertyAnimation::finished, [=]
		{
			showMinimized();
			setWindowOpacity(1.0);
		});
		setWindowState(Qt::WindowFullScreen);
	}
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
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_OPEN_PARAGRAPH, QString(), QString(), QVariant(), ISParagraphEntity::Instance().GetParagraph(ParagraphUID)->LocalName);
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
void ISMainWindow::ShowChangePasswordForm()
{
	if (ISMetaUser::Instance().UserData.System)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotChangeSystemUserPassword"));
		return;
	}

	if (!ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_CHANGE_PASSWORD))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.UserPasswordChange"));
		return;
	}

	SetVisibleShadow(true);
	if (ISGui::ShowUserPasswordForm(CURRENT_USER_ID))
	{
		ISMessageBox::ShowInformation(this, LANG("Message.Information.YouPasswordDoneChanged"));
	}
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
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_SHOW_ABOUT_FORM, QString(), QString(), QVariant());
	ISAboutForm().Exec();
	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAboutQt()
{
	SetVisibleShadow(true);
	QApplication::aboutQt();
	SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
