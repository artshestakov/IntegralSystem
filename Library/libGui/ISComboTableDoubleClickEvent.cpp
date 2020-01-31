#include "StdAfx.h"
#include "ISComboTableDoubleClickEvent.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTableDoubleClickEvent::ISComboTableDoubleClickEvent(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.Table.DoubleClickEvent.Window"), "Window");
	AddItem(LANG("Settings.Table.DoubleClickEvent.Tab"), "Tab");
}
//-----------------------------------------------------------------------------
ISComboTableDoubleClickEvent::~ISComboTableDoubleClickEvent()
{

}
//-----------------------------------------------------------------------------
