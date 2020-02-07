#include "ISAttachFileItem.h"
#include "ISButtons.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISAttachFileItem::ISAttachFileItem(QWidget *parent) : QWidget(parent)
{
	ItemIndex = -1;

	QHBoxLayout *Layout = new QHBoxLayout();
	setLayout(Layout);

	Label = new QLabel(this);
	Layout->addWidget(Label);

	ISButtonClear *ButtonClear = new ISButtonClear(this);
	ButtonClear->setToolTip(LANG("Delete"));
	connect(ButtonClear, &ISButtonClear::clicked, this, &ISAttachFileItem::Delete);
	Layout->addWidget(ButtonClear);
}
//-----------------------------------------------------------------------------
ISAttachFileItem::~ISAttachFileItem()
{

}
//-----------------------------------------------------------------------------
void ISAttachFileItem::SetFilePath(const QString &Text)
{
	Label->setText(Text);
}
//-----------------------------------------------------------------------------
QString ISAttachFileItem::GetFilePath() const
{
	return Label->text();
}
//-----------------------------------------------------------------------------
void ISAttachFileItem::SetItemIndex(int Index)
{
	ItemIndex = Index;
}
//-----------------------------------------------------------------------------
int ISAttachFileItem::GetItemIndex() const
{
	return ItemIndex;
}
//-----------------------------------------------------------------------------
void ISAttachFileItem::SetFont(const QFont &Font)
{
	Label->setFont(Font);
}
//-----------------------------------------------------------------------------
