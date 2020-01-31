#include "StdAfx.h"
#include "ISYearEdit.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISServiceButton.h"
//-----------------------------------------------------------------------------
ISYearEdit::ISYearEdit(QWidget *parent) : ISIntegerEdit(parent)
{
	SetMinimum(YEAR_MINIMUM);
	SetMaximum(YEAR_MAXIMUM);

	ISServiceButton *ButtonCurrentYear = new ISServiceButton(this);
	ButtonCurrentYear->setToolTip(LANG("CurrentYear"));
	ButtonCurrentYear->setIcon(BUFFER_ICONS("Calendar"));
	ButtonCurrentYear->setFocusPolicy(Qt::NoFocus);
	connect(ButtonCurrentYear, &ISServiceButton::clicked, this, &ISYearEdit::SelectCurrentYear);
	AddWidgetToRight(ButtonCurrentYear);
}
//-----------------------------------------------------------------------------
ISYearEdit::~ISYearEdit()
{

}
//-----------------------------------------------------------------------------
void ISYearEdit::SelectCurrentYear()
{
	SetValue(DATE_TODAY.year());
}
//-----------------------------------------------------------------------------
