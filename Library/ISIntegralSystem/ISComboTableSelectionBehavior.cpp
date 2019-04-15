#include "StdAfx.h"
#include "ISComboTableSelectionBehavior.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTableSelectionBehavior::ISComboTableSelectionBehavior(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LOCALIZATION("Settings.Table.SelectionBehavior.SelectItems"), "SelectItems");
	AddItem(LOCALIZATION("Settings.Table.SelectionBehavior.SelectRows"), "SelectRows");
	AddItem(LOCALIZATION("Settings.Table.SelectionBehavior.SelectColumns"), "SelectColumns");
}
//-----------------------------------------------------------------------------
ISComboTableSelectionBehavior::~ISComboTableSelectionBehavior()
{

}
//-----------------------------------------------------------------------------
