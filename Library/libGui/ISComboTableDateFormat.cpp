#include "StdAfx.h"
#include "ISComboTableDateFormat.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTableDateFormat::ISComboTableDateFormat(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.Table.DateFormat.Default"), "Default");
	AddItem(LANG("Settings.Table.DateFormat.Extended"), "Extended");
}
//-----------------------------------------------------------------------------
ISComboTableDateFormat::~ISComboTableDateFormat()
{

}
//-----------------------------------------------------------------------------
