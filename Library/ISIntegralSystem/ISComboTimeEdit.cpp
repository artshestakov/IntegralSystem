#include "StdAfx.h"
#include "ISComboTimeEdit.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTimeEdit::ISComboTimeEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	AddItem(LOCALIZATION("Minute_3"), 3);
	AddItem(LOCALIZATION("Minute_5"), 5);
	AddItem(LOCALIZATION("Minute_10"), 10);
	AddItem(LOCALIZATION("Minute_15"), 15);
	AddItem(LOCALIZATION("Minute_30"), 30);
	AddItem(LOCALIZATION("Minute_45"), 45);
	AddItem(LOCALIZATION("Minute_60"), 60);
	AddItem(LOCALIZATION("Minute_120"), 120);
	AddItem(LOCALIZATION("Minute_180"), 180);
	AddItem(LOCALIZATION("Minute_240"), 240);
	AddItem(LOCALIZATION("Minute_300"), 300);
	AddItem(LOCALIZATION("Minute_360"), 360);
}
//-----------------------------------------------------------------------------
ISComboTimeEdit::~ISComboTimeEdit()
{

}
//-----------------------------------------------------------------------------
