#include "ISSettingFIeldWidgets.h"
#include "ISLocalization.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISComboSettingEdit::ISComboSettingEdit(QWidget *parent) : ISComboEdit(parent)
{
    SetEditable(false);
}
//-----------------------------------------------------------------------------
ISComboSettingEdit::~ISComboSettingEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboCalendarNoteType::ISComboCalendarNoteType(QWidget *parent) : ISComboSettingEdit(parent)
{
    AddItem(LANG("Settings.CalendarEvents.NoteView.ToolTip"), "ToolTip");
    AddItem(LANG("Settings.CalendarEvents.NoteView.List"), "List");
}
//-----------------------------------------------------------------------------
ISComboCalendarNoteType::~ISComboCalendarNoteType()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
