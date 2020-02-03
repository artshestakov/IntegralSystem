#include "ISComboTimeEdit.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTimeEdit::ISComboTimeEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	AddItem(LANG("Minute_3"), 3);
	AddItem(LANG("Minute_5"), 5);
	AddItem(LANG("Minute_10"), 10);
	AddItem(LANG("Minute_15"), 15);
	AddItem(LANG("Minute_30"), 30);
	AddItem(LANG("Minute_45"), 45);
	AddItem(LANG("Minute_60"), 60);
	AddItem(LANG("Minute_120"), 120);
	AddItem(LANG("Minute_180"), 180);
	AddItem(LANG("Minute_240"), 240);
	AddItem(LANG("Minute_300"), 300);
	AddItem(LANG("Minute_360"), 360);
}
//-----------------------------------------------------------------------------
ISComboTimeEdit::~ISComboTimeEdit()
{

}
//-----------------------------------------------------------------------------
