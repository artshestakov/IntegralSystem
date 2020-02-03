#include "ISComboCalendarNoteType.h"
#include "ISLocalization.h"
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
