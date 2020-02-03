#include "ISDoubleEdit.h"
#include "ISDefines.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISDoubleEdit::ISDoubleEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	DoubleValue = 0;

	DoubleSpinBox = new ISQDoubleSpinBox(this);
	DoubleSpinBox->setStyleSheet(STYLE_SHEET("ISDoubleEdit"));
	DoubleSpinBox->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	DoubleSpinBox->setSpecialValueText(SYMBOL_SPACE_HIDE);
	DoubleSpinBox->setAccelerated(true);
	connect(DoubleSpinBox, static_cast<void(ISQDoubleSpinBox::*)(const QString &)>(&ISQDoubleSpinBox::valueChanged), this, &ISDoubleEdit::OnValueChanged);
	connect(DoubleSpinBox, &ISQDoubleSpinBox::ClearClicked, this, &ISDoubleEdit::Clear);
	AddWidgetEdit(DoubleSpinBox, this);

	SetFixedWidth(150);
}
//-----------------------------------------------------------------------------
ISDoubleEdit::~ISDoubleEdit()
{

}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetValue(const QVariant &value)
{
	DoubleSpinBox->setValue(value.toDouble());
	OnValueChanged(value.toString());
}
//-----------------------------------------------------------------------------
QVariant ISDoubleEdit::GetValue() const
{
	if (StringValue.length())
	{
		return DoubleValue;
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::Clear()
{
	DoubleSpinBox->clear();
	DoubleSpinBox->setSpecialValueText(SYMBOL_SPACE_HIDE);
	DoubleSpinBox->clearFocus();
	DoubleSpinBox->setFocus();
	StringValue.clear();
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetVisibleClear(bool Visible)
{
	DoubleSpinBox->SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetMinimum(double Minimum)
{
	DoubleSpinBox->setMinimum(Minimum);
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetMaximum(double Maximum)
{
	DoubleSpinBox->setMaximum(Maximum);
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetSuffix(const QString &Suffix)
{
	DoubleSpinBox->setSuffix(Suffix);
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetReadOnly(bool read_only)
{
	SetVisibleClear(!read_only);
	DoubleSpinBox->setReadOnly(read_only);

	if (read_only)
	{
		DoubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	}
	else
	{
		DoubleSpinBox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
	}
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::OnValueChanged(const QString &Text)
{
	StringValue = Text;
	DoubleValue = DoubleSpinBox->valueFromText(StringValue);
	if (StringValue.length()) //Если значение введено (любое)
	{
		if (StringValue == SYMBOL_0)
		{
			DoubleSpinBox->setSpecialValueText(QString());
		}
	}
	else
	{
		Clear();
	}

	ValueChanged();
}
//-----------------------------------------------------------------------------
