#include "ISSexEdit.h"
#include "ISQuery.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
static QString QS_SEX = PREPARE_QUERY("SELECT sexs_id, sexs_name "
									  "FROM _sex "
									  "WHERE NOT sexs_isdeleted "
									  "ORDER BY sexs_name");
//-----------------------------------------------------------------------------
ISSexEdit::ISSexEdit(QWidget *parent)
	: ISFieldEditBase(parent),
	CurrentID(0)
{
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	CreateButtonClear();

	ButtonGroup = new QButtonGroup(this);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &ISSexEdit::ButtonClicked);

	Widget = new QWidget(this);
	Widget->setLayout(new QHBoxLayout());
	Widget->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	AddWidgetEdit(Widget, this);

	ISQuery qSelectSex(QS_SEX);
	if (qSelectSex.Execute())
	{
		while (qSelectSex.Next())
		{
			ISPushButton *ButtonSex = new ISPushButton(qSelectSex.ReadColumn("sexs_name").toString(), Widget);
			ButtonSex->setCheckable(true);
			ButtonSex->setCursor(CURSOR_POINTING_HAND);
			Widget->layout()->addWidget(ButtonSex);
			ButtonGroup->addButton(ButtonSex, qSelectSex.ReadColumn("sexs_id").toInt());
		}
	}
}
//-----------------------------------------------------------------------------
ISSexEdit::~ISSexEdit()
{

}
//-----------------------------------------------------------------------------
void ISSexEdit::SetValue(const QVariant &value)
{
	bool Ok = true;
	CurrentID = value.toInt(&Ok);
	if (Ok)
	{	
		ButtonGroup->button(CurrentID)->setChecked(true);
	}
}
//-----------------------------------------------------------------------------
QVariant ISSexEdit::GetValue() const
{
	return CurrentID > 0 ? CurrentID : QVariant();
}
//-----------------------------------------------------------------------------
void ISSexEdit::Clear()
{
	QAbstractButton *AbstractButton = ButtonGroup->checkedButton();
	if (AbstractButton)
	{
		ButtonGroup->setExclusive(false);
		AbstractButton->setChecked(false);
		ButtonGroup->setExclusive(true);
		CurrentID = 0;
		emit ValueChanged();
	}
}
//-----------------------------------------------------------------------------
void ISSexEdit::SetFont(const QFont &Font)
{
	for (QAbstractButton *AbstractButton : ButtonGroup->buttons())
	{
		AbstractButton->setFont(Font);
	}
}
//-----------------------------------------------------------------------------
void ISSexEdit::SetReadOnly(bool read_only)
{
	ISFieldEditBase::SetReadOnly(read_only);
	Widget->setEnabled(!read_only);
}
//-----------------------------------------------------------------------------
void ISSexEdit::ButtonClicked(QAbstractButton *AbstractButton)
{
	CurrentID = ButtonGroup->id(AbstractButton);
	emit ValueChanged();
}
//-----------------------------------------------------------------------------
