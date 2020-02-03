#include "ISComboTabBarSelectionBehavior.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTabBarSelectionBehavior::ISComboTabBarSelectionBehavior(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.TabBar.SelectionBehavior.SelectLeftTab"), "SelectLeftTab");
	AddItem(LANG("Settings.TabBar.SelectionBehavior.SelectRightTab"), "SelectRightTab");
	AddItem(LANG("Settings.TabBar.SelectionBehavior.SelectPreviousTab"), "SelectPreviousTab");
}
//-----------------------------------------------------------------------------
ISComboTabBarSelectionBehavior::~ISComboTabBarSelectionBehavior()
{

}
//-----------------------------------------------------------------------------
