#include "StdAfx.h"
#include "ISLineEdit.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISLineEdit::ISLineEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	SetSizePolicyHorizontal(QSizePolicy::Minimum);

	LineEdit = new ISQLineEdit(this);
	connect(LineEdit, &ISQLineEdit::ClearClicked, this, &ISLineEdit::Clear);
	connect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::ValueChanged);
	AddWidgetEdit(LineEdit, this);
}
//-----------------------------------------------------------------------------
ISLineEdit::~ISLineEdit()
{

}
//-----------------------------------------------------------------------------
void ISLineEdit::SetValue(const QVariant &value)
{
	LineEdit->setText(value.toString());
}
//-----------------------------------------------------------------------------
QVariant ISLineEdit::GetValue() const
{
	if (LineEdit->text().length())
	{
		return LineEdit->text();
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISLineEdit::Clear()
{
	LineEdit->clear();
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetVisibleClear(bool Visible)
{
	LineEdit->SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetRegExp(const QString &RegExp)
{
	SetValidator(new QRegExpValidator(QRegExp(RegExp), this));
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetValidator(QValidator *Validator)
{
	LineEdit->setValidator(Validator);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetEchoMode(QLineEdit::EchoMode EchoMode)
{
	LineEdit->setEchoMode(EchoMode);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetCompleter(QCompleter *Completer)
{
	LineEdit->setCompleter(Completer);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetTextAlignment(Qt::Alignment Alignment)
{
	LineEdit->setAlignment(Alignment);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetUppercase(bool uppercase)
{
	if (uppercase)
	{
		connect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnUpperText);
	}
	else
	{
		disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnUpperText);
	}
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetLowercase(bool lowercase)
{
	if (lowercase)
	{
		connect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnLowerText);
	}
	else
	{
		disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnLowerText);
	}
}
//-----------------------------------------------------------------------------
void ISLineEdit::ResetFontcase()
{
	disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnUpperText);
	disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnLowerText);
}
//-----------------------------------------------------------------------------
void ISLineEdit::AddAction(QAction *Action, QLineEdit::ActionPosition Position)
{
	LineEdit->AddAction(Action, Position);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetReadOnly(bool read_only)
{
	SetVisibleClear(false);
	LineEdit->setReadOnly(read_only);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetPlaceholderText(const QString &placeholder_text)
{
	LineEdit->setPlaceholderText(placeholder_text);
}
//-----------------------------------------------------------------------------
void ISLineEdit::OnUpperText(const QString &Text)
{
	int Position = LineEdit->cursorPosition();
	LineEdit->setText(Text.toUpper());
	LineEdit->setCursorPosition(Position);
}
//-----------------------------------------------------------------------------
void ISLineEdit::OnLowerText(const QString &Text)
{
	int Position = LineEdit->cursorPosition();
	LineEdit->setText(Text.toLower());
	LineEdit->setCursorPosition(Position);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetInputMask(const QString &InputMask)
{
	LineEdit->setInputMask(InputMask);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetFocusPolicy(Qt::FocusPolicy focus_policy)
{
	LineEdit->setFocusPolicy(focus_policy);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetMaxLength(int Length)
{
	LineEdit->setMaxLength(Length);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetIcon(const QIcon &Icon)
{
	LineEdit->SetIcon(Icon);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SelectAll()
{
	QTimer::singleShot(10, GetLineEdit(), &ISQLineEdit::selectAll);
}
//-----------------------------------------------------------------------------
ISQLineEdit* ISLineEdit::GetLineEdit()
{
	return LineEdit;
}
//-----------------------------------------------------------------------------
