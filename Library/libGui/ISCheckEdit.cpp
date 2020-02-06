#include "ISCheckEdit.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISCheckEdit::ISCheckEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	CheckBox = new QCheckBox(this);
	CheckBox->setChecked(false);
	CheckBox->setTristate(false);
	CheckBox->setCursor(CURSOR_POINTING_HAND);
	connect(CheckBox, &QCheckBox::stateChanged, this, &ISCheckEdit::ValueChanged);
	AddWidgetEdit(CheckBox, this);
	AddWidgetEditStretch();
}
//-----------------------------------------------------------------------------
ISCheckEdit::~ISCheckEdit()
{

}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetValue(const QVariant &value)
{
	CheckBox->setChecked(value.toBool());
}
//-----------------------------------------------------------------------------
QVariant ISCheckEdit::GetValue() const
{
	return CheckBox->isChecked();
}
//-----------------------------------------------------------------------------
void ISCheckEdit::Clear()
{
	ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetVisibleClear(bool Visible)
{
	ISFieldEditBase::SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetText(const QString &Text)
{
	CheckBox->setText(Text);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetReadOnly(bool ReadOnly)
{
	CheckBox->setEnabled(!ReadOnly);
}
//-----------------------------------------------------------------------------
QCheckBox* ISCheckEdit::GetCheckBox()
{
	return CheckBox;
}
//-----------------------------------------------------------------------------
