#include "ISLockForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaUser.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISProtocol.h"
#include "ISNotifyRecipient.h"
//-----------------------------------------------------------------------------
ISLockForm::ISLockForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	IsClose = false;
	NewNotifyCount = 0;

	setWindowIcon(BUFFER_ICONS("Lock"));
	setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::Dialog);
	ForbidResize();

	connect(&ISNotifyRecipient::GetInstance(), static_cast<void(ISNotifyRecipient::*)(void)>(&ISNotifyRecipient::Notify), this, &ISLockForm::NewNotify);
	connect(this, &ISLockForm::AnimationFinished, this, &ISLockForm::close);

	QLabel *LabelLogo = new QLabel(this);
	LabelLogo->setPixmap(BUFFER_PIXMAPS("BannerText").scaled(ISDefines::Gui::SIZE_450_450, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	GetMainLayout()->addWidget(LabelLogo);

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	GetMainLayout()->addLayout(Layout);

	QLabel *LabelLock = new QLabel(this);
	LabelLock->setWordWrap(true);
	LabelLock->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LabelLock->setText(LANG("ApplicationLock"));
	Layout->addWidget(LabelLock);

	QFormLayout *FormLayout = new QFormLayout();

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setLayout(FormLayout);
	Layout->addWidget(GroupBox);

	QLabel *LabelUser = new QLabel(this);
	LabelUser->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LabelUser->setText(ISMetaUser::GetInstance().GetData()->FullName);
	FormLayout->addRow(LANG("User") + ':', LabelUser);

	EditPassword = new ISPasswordEdit(this);
	EditPassword->SetVisibleGenerate(false);
	FormLayout->addRow(LANG("Password") + ':', EditPassword);

	LabelNotify = new QLabel(this);
	ISGui::SetFontWidgetBold(LabelNotify, true);
	Layout->addWidget(LabelNotify, 0, Qt::AlignLeft);

	ISPushButton *ButtonUnlock = new ISPushButton(this);
	ButtonUnlock->setText(LANG("Unlock"));
	ButtonUnlock->setIcon(BUFFER_ICONS("Arrow.Right"));
	connect(ButtonUnlock, &ISPushButton::clicked, this, &ISLockForm::Unlock);
	Layout->addWidget(ButtonUnlock, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISLockForm::~ISLockForm()
{

}
//-----------------------------------------------------------------------------
void ISLockForm::closeEvent(QCloseEvent *e)
{
	if (IsClose)
	{
		ISInterfaceDialogForm::closeEvent(e);
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotCloseLockForm"));
		e->ignore();
	}
}
//-----------------------------------------------------------------------------
void ISLockForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	EditPassword->SetFocus();
}
//-----------------------------------------------------------------------------
void ISLockForm::Unlock()
{
	ISGui::SetWaitGlobalCursor(true);

	QString EnterPassword = EditPassword->GetValue().toString();
	if (EnterPassword.length())
	{
		if (ISMetaUser::GetInstance().CheckPassword(EnterPassword))
		{
			SetResult(true);
			IsClose = true;

			HideAnimation();
			emit Unlocked();
		}
		else
		{
			ISGui::SetWaitGlobalCursor(false);
			ISProtocol::Insert(true, CONST_UID_PROTOCOL_INVALID_UNLOCK_APPLICATION, QString(), QString(), QVariant(), LANG("IncorrectPassword"));
			ISMessageBox::ShowWarning(this, LANG("Message.Password.Error"));
			EditPassword->BlinkRed();
		}
	}
	else
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Password.IsEmpty"));
		EditPassword->BlinkRed();
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISLockForm::EnterClicked()
{
	Unlock();
}
//-----------------------------------------------------------------------------
void ISLockForm::NewNotify()
{
	++NewNotifyCount;
	LabelNotify->setText(LANG("NewNotifications").arg(NewNotifyCount));
}
//-----------------------------------------------------------------------------
