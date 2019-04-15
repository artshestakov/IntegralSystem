#include "StdAfx.h"
#include "ISMonthEdit.h"
#include "EXDefines.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISMonthEdit::ISMonthEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	AddItem(LOCALIZATION("Month.January"), 1);
	AddItem(LOCALIZATION("Month.February"), 2);
	AddItem(LOCALIZATION("Month.March"), 3);
	AddItem(LOCALIZATION("Month.April"), 4);
	AddItem(LOCALIZATION("Month.May"), 5);
	AddItem(LOCALIZATION("Month.June"), 6);
	AddItem(LOCALIZATION("Month.July"), 7);
	AddItem(LOCALIZATION("Month.August"), 8);
	AddItem(LOCALIZATION("Month.September"), 9);
	AddItem(LOCALIZATION("Month.October"), 10);
	AddItem(LOCALIZATION("Month.November"), 11);
	AddItem(LOCALIZATION("Month.December"), 12);
}
//-----------------------------------------------------------------------------
ISMonthEdit::~ISMonthEdit()
{

}
//-----------------------------------------------------------------------------
void ISMonthEdit::SelectCurrentMonth()
{
	SetValue(DATE_TODAY.month());
}
//-----------------------------------------------------------------------------
