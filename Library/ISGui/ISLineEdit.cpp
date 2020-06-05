#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
ISLineEdit::ISLineEdit(QWidget *parent)
	: ISFieldEditBase(parent),
	Completer(nullptr)
{
	LineEdit = new ISQLineEdit(this);
	connect(LineEdit, &ISQLineEdit::ClearClicked, this, &ISLineEdit::Clear);
	connect(LineEdit, &ISQLineEdit::returnPressed, this, &ISLineEdit::EnterClicked);
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
	return LineEdit->text().isEmpty() ? QVariant() : LineEdit->text();
}
//-----------------------------------------------------------------------------
void ISLineEdit::Clear()
{
	LineEdit->clear();
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetRegExp(const QString &RegExp)
{
	SetValidator(new QRegExpValidator(QRegExp(RegExp), this));
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetColorText(const QColor &Color)
{
	QPalette Palette = LineEdit->palette();
	Palette.setColor(QPalette::Text, Color);
	LineEdit->setPalette(Palette);
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
	uppercase ?
		connect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnUpperText) :
		disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnUpperText);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetLowercase(bool lowercase)
{
	lowercase ?
		connect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnLowerText) :
		disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnLowerText);
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
void ISLineEdit::CreateCompleter(const QStringList &StringList)
{
	if (Completer)
	{
		delete Completer;
		Completer = nullptr;
	}

	Completer = new QCompleter(StringList, this);
	Completer->setMaxVisibleItems(20);
	Completer->setFilterMode(Qt::MatchContains);
	Completer->setCaseSensitivity(Qt::CaseInsensitive);
	Completer->setCompletionMode(QCompleter::PopupCompletion);
	SetCompleter(Completer);
}
//-----------------------------------------------------------------------------
QStringList ISLineEdit::GetCompleterList() const
{
	QStringList StringList;
	for (int i = 0; i < Completer->model()->rowCount(); ++i)
	{
		StringList.append(Completer->model()->data(Completer->model()->index(i, 0)).toString());
	}
	return StringList;
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetReadOnly(bool read_only)
{
	SetVisibleClear(!read_only);
	LineEdit->setReadOnly(read_only);
	LineEdit->SetVisibleClear(!read_only);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetPlaceholderText(const QString &placeholder_text)
{
	LineEdit->setPlaceholderText(placeholder_text);
}
//-----------------------------------------------------------------------------
void ISLineEdit::OnUpperText(const QString &Text)
{
	LineEdit->setText(Text.toUpper());
	LineEdit->setCursorPosition(LineEdit->cursorPosition());
}
//-----------------------------------------------------------------------------
void ISLineEdit::OnLowerText(const QString &Text)
{
	LineEdit->setText(Text.toLower());
	LineEdit->setCursorPosition(LineEdit->cursorPosition());
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
