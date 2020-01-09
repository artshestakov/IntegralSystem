#include "StdAfx.h"
#include "ISIntegerEdit.h"
#include "ISDefines.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISIntegerEdit::ISIntegerEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	SpinBox = new ISQSpinBox(this);
	SpinBox->setStyleSheet(STYLE_SHEET("ISIntegerEdit"));
	SpinBox->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	SpinBox->setSpecialValueText(SYMBOL_SPACE_HIDE);
	SpinBox->setAccelerated(true);
	connect(SpinBox, static_cast<void(ISQSpinBox::*)(const QString &)>(&ISQSpinBox::valueChanged), this, &ISIntegerEdit::OnValueChanged);
	connect(SpinBox, &ISQSpinBox::ClearClicked, this, &ISIntegerEdit::Clear);
	AddWidgetEdit(SpinBox, this);

	SetFixedWidth(150);
}
//-----------------------------------------------------------------------------
ISIntegerEdit::~ISIntegerEdit()
{

}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetValue(const QVariant &value)
{
	SpinBox->setValue(value.toInt());
	OnValueChanged(value.toString());
}
//-----------------------------------------------------------------------------
QVariant ISIntegerEdit::GetValue() const
{
	if (IntValue.length())
	{
		return SpinBox->value();
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::Clear()
{
	SpinBox->clear();
	SpinBox->setSpecialValueText(SYMBOL_SPACE_HIDE);
	SpinBox->clearFocus();
	SpinBox->setFocus();
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetVisibleClear(bool Visible)
{
	SpinBox->SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetMinimum(int Minimum)
{
	SpinBox->setMinimum(Minimum);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetMaximum(int Maximum)
{
	SpinBox->setMaximum(Maximum);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetReadOnly(bool ReadOnly)
{
	SpinBox->setReadOnly(ReadOnly);
	SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::OnValueChanged(const QString &Text)
{
	if (Text.length()) //Если значение введено (любое)
	{
		if (Text == SYMBOL_0) //Если ввели ноль, убрать спец. значение чтобы ноль отображался
		{
			SpinBox->setSpecialValueText(QString());
		}
	}
	else //Значение отсутствует (удалили все символы)
	{
		Clear();
	}

	IntValue = Text;
	SpinBox->SetCursorPosition(Text.length());
	ValueChanged();
}
//-----------------------------------------------------------------------------
