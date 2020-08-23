#include "ISCheckEdit.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISCheckEdit::ISCheckEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	SetSizePolicyHorizontal(QSizePolicy::Maximum);

	CheckBox = new QCheckBox(this);
	CheckBox->setChecked(false);
	CheckBox->setTristate(false);
	CheckBox->setCursor(CURSOR_POINTING_HAND);
	connect(CheckBox, &QCheckBox::stateChanged, this, &ISCheckEdit::ValueChanged);
	AddWidgetEdit(CheckBox, this);
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
void ISCheckEdit::SetReadOnly(bool read_only)
{
	CheckBox->setEnabled(!read_only);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetText(const QString &Text)
{
	CheckBox->setText(Text);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetCheckableStrikeOut(bool StrikeOut)
{
	CheckableStrikeOut(); //Вызываем на всякий случай, вдруг сейчас CheckEdit уже активен
	StrikeOut ?
		connect(CheckBox, &QCheckBox::stateChanged, this, &ISCheckEdit::CheckableStrikeOut) :
		disconnect(CheckBox, &QCheckBox::stateChanged, this, &ISCheckEdit::CheckableStrikeOut);
}
//-----------------------------------------------------------------------------
QCheckBox* ISCheckEdit::GetCheckBox()
{
	return CheckBox;
}
//-----------------------------------------------------------------------------
void ISCheckEdit::CheckableStrikeOut()
{
	QFont Font = CheckBox->font();
	Font.setStrikeOut(CheckBox->isChecked());
	CheckBox->setFont(Font);
}
//-----------------------------------------------------------------------------
