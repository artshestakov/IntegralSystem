#include "StdAfx.h"
#include "ISComboTabBarSelectionBehavior.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboTabBarSelectionBehavior::ISComboTabBarSelectionBehavior(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LOCALIZATION("Settings.TabBar.SelectionBehavior.SelectLeftTab"), "SelectLeftTab");
	AddItem(LOCALIZATION("Settings.TabBar.SelectionBehavior.SelectRightTab"), "SelectRightTab");
	AddItem(LOCALIZATION("Settings.TabBar.SelectionBehavior.SelectPreviousTab"), "SelectPreviousTab");
}
//-----------------------------------------------------------------------------
ISComboTabBarSelectionBehavior::~ISComboTabBarSelectionBehavior()
{

}
//-----------------------------------------------------------------------------
