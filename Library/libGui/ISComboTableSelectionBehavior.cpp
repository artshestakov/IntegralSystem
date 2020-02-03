#include "ISComboTableSelectionBehavior.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTableSelectionBehavior::ISComboTableSelectionBehavior(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.Table.SelectionBehavior.SelectItems"), "SelectItems");
	AddItem(LANG("Settings.Table.SelectionBehavior.SelectRows"), "SelectRows");
	AddItem(LANG("Settings.Table.SelectionBehavior.SelectColumns"), "SelectColumns");
}
//-----------------------------------------------------------------------------
ISComboTableSelectionBehavior::~ISComboTableSelectionBehavior()
{

}
//-----------------------------------------------------------------------------
