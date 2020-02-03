#include "ISPathEditFile.h"
#include "ISPushButton.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISPathEditFile::ISPathEditFile(QWidget *parent) : ISLineEdit(parent)
{
	SetReadOnly(true);
	SetVisibleClear(true); //„тобы кнопка очистки отображалась, пор€док SetReadOnly и SetVisibleClear доллжен быть именно такой
	SetFocusPolicy(Qt::NoFocus);
	SetPlaceholderText(LANG("ClickOverviewFromSelectFile"));

	ISPushButton *ButtonSelect = new ISPushButton(this);
	ButtonSelect->setText(LANG("Overview") + "...");
	ButtonSelect->setToolTip(LANG("SelectFile"));
	connect(ButtonSelect, &ISPushButton::clicked, this, &ISPathEditFile::SelectFile);
	AddWidgetToRight(ButtonSelect);
}
//-----------------------------------------------------------------------------
ISPathEditFile::~ISPathEditFile()
{

}
//-----------------------------------------------------------------------------
void ISPathEditFile::SetValue(const QVariant &value)
{
	ISLineEdit::SetValue(value);
	SetToolTip(value.toString());
}
//-----------------------------------------------------------------------------
void ISPathEditFile::SetFilterFile(const QString &filter_file)
{
	FilterFile = filter_file;
}
//-----------------------------------------------------------------------------
void ISPathEditFile::SelectFile()
{
	QString Path = ISFileDialog::GetOpenFileName(this, GetValue().toString(), FilterFile);
	if (Path.length())
	{
		SetValue(Path);
	}
}
//-----------------------------------------------------------------------------
