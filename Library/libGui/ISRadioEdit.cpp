#include "ISRadioEdit.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISRadioEdit::ISRadioEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	LayoutPanel = new QHBoxLayout();
	LayoutPanel->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	WidgetPanel = new QWidget(this);
	WidgetPanel->setLayout(LayoutPanel);
	AddWidgetEdit(WidgetPanel, this);

	ButtonGroup = new QButtonGroup(this);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &ISRadioEdit::ValueChanged);
}
//-----------------------------------------------------------------------------
ISRadioEdit::~ISRadioEdit()
{

}
//-----------------------------------------------------------------------------
void ISRadioEdit::SetValue(const QVariant &value)
{
	ButtonGroup->button(MapButtons.key(value))->setChecked(true);
}
//-----------------------------------------------------------------------------
QVariant ISRadioEdit::GetValue() const
{
	QVariant Value = MapButtons.value(ButtonGroup->checkedId());
	if (Value.isValid())
	{
		return Value;
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISRadioEdit::Clear()
{
	ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
void ISRadioEdit::AddButton(QRadioButton *RadioButton, const QVariant &Value)
{
	RadioButton->setParent(WidgetPanel);
	RadioButton->setCursor(CURSOR_POINTING_HAND);
	LayoutPanel->addWidget(RadioButton);

	int ButtonID = ButtonGroup->buttons().count();
	ButtonGroup->addButton(RadioButton, ButtonID);
	MapButtons.insert(ButtonID, Value);
}
//-----------------------------------------------------------------------------
