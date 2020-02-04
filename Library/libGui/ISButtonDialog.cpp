#include "ISButtonDialog.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "EXDefines.h"
//-----------------------------------------------------------------------------
ISButtonDialog::ISButtonDialog(QWidget *parent, const QString &ApplyText, const QString &CloseText) : QDialogButtonBox(parent)
{
	setOrientation(Qt::Horizontal);
	setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	button(QDialogButtonBox::Ok)->setIcon(BUFFER_ICONS("Apply.Blue"));
	button(QDialogButtonBox::Ok)->setStyleSheet(STYLE_SHEET("ISPushButton"));
	button(QDialogButtonBox::Ok)->setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
	connect(button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &ISButtonDialog::Apply);

	button(QDialogButtonBox::Cancel)->setIcon(BUFFER_ICONS("Close"));
	button(QDialogButtonBox::Cancel)->setStyleSheet(STYLE_SHEET("ISPushButton"));
	button(QDialogButtonBox::Cancel)->setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
	connect(button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &ISButtonDialog::Close);

	if (ApplyText.length())
	{
		button(QDialogButtonBox::Ok)->setText(ApplyText);
	}
	else
	{
		button(QDialogButtonBox::Ok)->setText(LANG("Apply"));
	}

	if (CloseText.length())
	{
		button(QDialogButtonBox::Cancel)->setText(CloseText);
	}
	else
	{
		button(QDialogButtonBox::Cancel)->setText(LANG("Close"));
	}
}
//-----------------------------------------------------------------------------
ISButtonDialog::~ISButtonDialog()
{

}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyIcon(const QIcon &ApplyIcon)
{
	button(QDialogButtonBox::Ok)->setIcon(ApplyIcon);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyEnabled(bool Enabled)
{
	button(QDialogButtonBox::Ok)->setEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyVisible(bool Visible)
{
	button(QDialogButtonBox::Ok)->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyCursor(const QCursor &Cursor)
{
	button(QDialogButtonBox::Ok)->setCursor(Cursor);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyFocus()
{
	button(QDialogButtonBox::Ok)->setFocus();
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetCloseIcon(const QIcon &Icon)
{
	button(QDialogButtonBox::Cancel)->setIcon(Icon);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetCloseEnabled(bool Enabled)
{
	button(QDialogButtonBox::Cancel)->setEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetCloseCursor(const QCursor &Cursor)
{
	button(QDialogButtonBox::Cancel)->setCursor(Cursor);
}
//-----------------------------------------------------------------------------
