#include "StdAfx.h"
#include "ISComboTableDateFormat.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTableDateFormat::ISComboTableDateFormat(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LOCALIZATION("Settings.Table.DateFormat.Default"), "Default");
	AddItem(LOCALIZATION("Settings.Table.DateFormat.Extended"), "Extended");
}
//-----------------------------------------------------------------------------
ISComboTableDateFormat::~ISComboTableDateFormat()
{

}
//-----------------------------------------------------------------------------
