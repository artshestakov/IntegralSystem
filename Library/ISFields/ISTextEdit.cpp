#include "StdAfx.h"
#include "ISTextEdit.h"
#include "ISAssert.h"
#include "EXDefines.h"
//-----------------------------------------------------------------------------
ISTextEdit::ISTextEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	SetSizePolicyHorizontal(QSizePolicy::Minimum);
	CreateButtonClear();

	TextEdit = new ISQTextEdit(this);
	TextEdit->setTabChangesFocus(true);
	TextEdit->setAcceptRichText(false);
	connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::ValueChanged);
	connect(TextEdit, &ISQTextEdit::KeyPressEnter, this, &ISTextEdit::KeyPressEnter);
	AddWidgetEdit(TextEdit, this);
}
//-----------------------------------------------------------------------------
ISTextEdit::~ISTextEdit()
{

}
//-----------------------------------------------------------------------------
void ISTextEdit::SetValue(const QVariant &value)
{
	TextEdit->setText(value.toString());
}
//-----------------------------------------------------------------------------
QVariant ISTextEdit::GetValue() const
{
	if (TextEdit->toPlainText().length())
	{
		return TextEdit->toPlainText();
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISTextEdit::Clear()
{
	TextEdit->clear();
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetVisibleClear(bool Visible)
{
	GetButtonClear()->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISTextEdit::AddText(const QString &Text)
{
	TextEdit->append(Text);
	TextEdit->verticalScrollBar()->setValue(TextEdit->verticalScrollBar()->maximum());
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetExecuteEnter(bool Execute)
{
	TextEdit->SetExecuteEnter(Execute);
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetUppercase(bool uppercase)
{
	if (uppercase)
	{
		connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);
	}
	else
	{
		disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);
	}
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetLowercase(bool lowercase)
{
	if (lowercase)
	{
		connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);
	}
	else
	{
		disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);
	}
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetReadOnly(bool ReadOnly)
{
	ISFieldEditBase::SetReadOnly(ReadOnly);
	TextEdit->setReadOnly(ReadOnly);
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetPlaceholderText(const QString &placeholder_text)
{
	TextEdit->setPlaceholderText(placeholder_text);
}
//-----------------------------------------------------------------------------
void ISTextEdit::OnUpperText()
{
	disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);

	QTextCursor TextCursor = TextEdit->textCursor();
	int Position = TextCursor.position();
	TextEdit->setText(TextEdit->toPlainText().toUpper());
	TextCursor.setPosition(Position);
	TextEdit->setTextCursor(TextCursor);

	connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);
}
//-----------------------------------------------------------------------------
void ISTextEdit::OnLowerText()
{
	disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);

	QTextCursor TextCursor = TextEdit->textCursor();
	int Position = TextCursor.position();
	TextEdit->setText(TextEdit->toPlainText().toLower());
	TextCursor.setPosition(Position);
	TextEdit->setTextCursor(TextCursor);

	connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);
}
//-----------------------------------------------------------------------------
void ISTextEdit::ResetFontcase()
{
	disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);
	disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);
}
//-----------------------------------------------------------------------------
ISQTextEdit* ISTextEdit::GetTextEdit()
{
	return TextEdit;
}
//-----------------------------------------------------------------------------
