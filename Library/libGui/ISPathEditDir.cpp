#include "StdAfx.h"
#include "ISPathEditDir.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISFileDialog.h"
#include "ISSystem.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
ISPathEditDir::ISPathEditDir(QWidget *parent) : ISLineEdit(parent)
{
	SetReadOnly(true);
	SetVisibleClear(true); //„тобы кнопка очистки отображалась, пор€док SetReadOnly и SetVisibleClear доллжен быть именно такой
	SetFocusPolicy(Qt::NoFocus);

	ISPushButton *ButtonSelect = new ISPushButton(this);
	ButtonSelect->setText(LOCALIZATION("Overview") + "...");
	ButtonSelect->setToolTip(LOCALIZATION("SelectFolder"));
	connect(ButtonSelect, &ISPushButton::clicked, this, &ISPathEditDir::SelectDir);
	AddWidgetToRight(ButtonSelect);

	ButtonOpenDir = new ISServiceButton(this);
	ButtonOpenDir->setIcon(BUFFER_ICONS("FolderClosed"));
	ButtonOpenDir->setToolTip(LOCALIZATION("OpenFolder"));
	ButtonOpenDir->setEnabled(false);
	connect(ButtonOpenDir, &ISPushButton::clicked, this, &ISPathEditDir::OpenDir);
	AddWidgetToRight(ButtonOpenDir);
}
//-----------------------------------------------------------------------------
ISPathEditDir::~ISPathEditDir()
{

}
//-----------------------------------------------------------------------------
void ISPathEditDir::SetValue(const QVariant &value)
{
	ISLineEdit::SetValue(value);
	SetToolTip(value.toString());

	if (value.toString().length())
	{
		ButtonOpenDir->setEnabled(true);
	}
	else
	{
		ButtonOpenDir->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISPathEditDir::Clear()
{
	ISLineEdit::Clear();
	ButtonOpenDir->setEnabled(false);
}
//-----------------------------------------------------------------------------
void ISPathEditDir::SelectDir()
{
	QString FolderPath = ISFileDialog::GetDirectoryPath(this);
	if (FolderPath.length())
	{
		SetValue(FolderPath);
	}
}
//-----------------------------------------------------------------------------
void ISPathEditDir::OpenDir()
{
	if (!ISSystem::OpenFolder(GetValue().toString()))
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Error.ErrorOpenedFolderPath").arg(GetValue().toString()));
	}
}
//-----------------------------------------------------------------------------
