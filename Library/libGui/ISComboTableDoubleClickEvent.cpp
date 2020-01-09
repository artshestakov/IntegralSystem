#include "StdAfx.h"
#include "ISComboTableDoubleClickEvent.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTableDoubleClickEvent::ISComboTableDoubleClickEvent(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LOCALIZATION("Settings.Table.DoubleClickEvent.Window"), "Window");
	AddItem(LOCALIZATION("Settings.Table.DoubleClickEvent.Tab"), "Tab");
}
//-----------------------------------------------------------------------------
ISComboTableDoubleClickEvent::~ISComboTableDoubleClickEvent()
{

}
//-----------------------------------------------------------------------------
