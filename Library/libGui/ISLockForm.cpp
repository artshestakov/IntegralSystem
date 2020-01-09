#include "StdAfx.h"
#include "ISLockForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaUser.h"
#include "ISMessageBox.h"
#include "ISSystem.h"
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
	LabelLogo->setPixmap(BUFFER_PIXMAPS("BannerText").scaled(SIZE_450_450, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	GetMainLayout()->addWidget(LabelLogo);

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(LAYOUT_MARGINS_5_PX);
	GetMainLayout()->addLayout(Layout);

	QLabel *LabelLock = new QLabel(this);
	LabelLock->setWordWrap(true);
	LabelLock->setFont(FONT_APPLICATION_BOLD);
	LabelLock->setText(LOCALIZATION("ApplicationLock"));
	Layout->addWidget(LabelLock);

	QFormLayout *FormLayout = new QFormLayout();

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setLayout(FormLayout);
	Layout->addWidget(GroupBox);

	QLabel *LabelUser = new QLabel(this);
	LabelUser->setFont(FONT_APPLICATION_BOLD);
	LabelUser->setText(CURRENT_USER_FULL_NAME);
	FormLayout->addRow(LOCALIZATION("User") + ":", LabelUser);

	EditPassword = new ISPasswordEdit(this);
	EditPassword->SetVisibleGenerate(false);
	FormLayout->addRow(LOCALIZATION("Password") + ":", EditPassword);

	LabelNotify = new QLabel(this);
	ISSystem::SetFontWidgetBold(LabelNotify, true);
	Layout->addWidget(LabelNotify, 0, Qt::AlignLeft);

	ISPushButton *ButtonUnlock = new ISPushButton(this);
	ButtonUnlock->setText(LOCALIZATION("Unlock"));
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
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotCloseLockForm"));
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
	ISSystem::SetWaitGlobalCursor(true);

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
			ISSystem::SetWaitGlobalCursor(false);
			ISProtocol::Insert(true, CONST_UID_PROTOCOL_INVALID_UNLOCK_APPLICATION, QString(), QString(), QVariant(), LOCALIZATION("IncorrectPassword"));
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Password.Error"));
			EditPassword->BlinkRed();
		}
	}
	else
	{
		ISSystem::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Error.Password.IsEmpty"));
		EditPassword->BlinkRed();
	}

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISLockForm::EnterClicked()
{
	Unlock();
}
//-----------------------------------------------------------------------------
void ISLockForm::NewNotify()
{
	NewNotifyCount++;
	LabelNotify->setText(LOCALIZATION("NewNotifications").arg(NewNotifyCount));
}
//-----------------------------------------------------------------------------
