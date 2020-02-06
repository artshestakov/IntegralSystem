#include "ISMonthEdit.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISMonthEdit::ISMonthEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	AddItem(LANG("Month.January"), 1);
	AddItem(LANG("Month.February"), 2);
	AddItem(LANG("Month.March"), 3);
	AddItem(LANG("Month.April"), 4);
	AddItem(LANG("Month.May"), 5);
	AddItem(LANG("Month.June"), 6);
	AddItem(LANG("Month.July"), 7);
	AddItem(LANG("Month.August"), 8);
	AddItem(LANG("Month.September"), 9);
	AddItem(LANG("Month.October"), 10);
	AddItem(LANG("Month.November"), 11);
	AddItem(LANG("Month.December"), 12);
}
//-----------------------------------------------------------------------------
ISMonthEdit::~ISMonthEdit()
{

}
//-----------------------------------------------------------------------------
void ISMonthEdit::SelectCurrentMonth()
{
	SetValue(QDate::currentDate().month());
}
//-----------------------------------------------------------------------------
