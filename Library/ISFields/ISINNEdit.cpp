#include "StdAfx.h"
#include "ISINNEdit.h"
#include "EXDefines.h"
#include "ISServiceButton.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISINNEdit::ISINNEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetPlaceholderText(LOCALIZATION("Field.Inn.PlaceholderText"));
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_12), this));

	ISServiceButton *ButtonSearch = new ISServiceButton(this);
	ButtonSearch->setToolTip(LOCALIZATION("SearchFromINN"));
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
	int CountSymbols = GetValue().toString().length();
	if (CountSymbols == 10 || CountSymbols == 12)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
