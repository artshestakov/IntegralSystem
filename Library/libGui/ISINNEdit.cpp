#include "ISINNEdit.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISINNEdit::ISINNEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetPlaceholderText(LANG("Field.Inn.PlaceholderText"));
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_12), this));

	ButtonSearch = new ISServiceButton(this);
	ButtonSearch->setVisible(false);
	ButtonSearch->setToolTip(LANG("SearchFromINN"));
	ButtonSearch->setIcon(BUFFER_ICONS("Taxation"));
	ButtonSearch->setFlat(true);
	ButtonSearch->setFocusPolicy(Qt::NoFocus);
	connect(ButtonSearch, &ISServiceButton::clicked, this, &ISINNEdit::SearchFromINN);
	AddWidgetToRight(ButtonSearch);
}
//-----------------------------------------------------------------------------
ISINNEdit::~ISINNEdit()
{

}
//-----------------------------------------------------------------------------
bool ISINNEdit::IsValid() const
{
	int Count = GetValue().toString().length();
	return !Count || Count == 10 || Count == 12;
}
//-----------------------------------------------------------------------------
void ISINNEdit::SetEnabledSearch(bool Enabled)
{
	ButtonSearch->setVisible(Enabled);
}
//-----------------------------------------------------------------------------
