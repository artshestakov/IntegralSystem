#include "StdAfx.h"
#include "ISComboCalendarNoteType.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboCalendarNoteType::ISComboCalendarNoteType(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LOCALIZATION("Settings.CalendarEvents.NoteView.ToolTip"), "ToolTip");
	AddItem(LOCALIZATION("Settings.CalendarEvents.NoteView.List"), "List");
}
//-----------------------------------------------------------------------------
ISComboCalendarNoteType::~ISComboCalendarNoteType()
{

}
//-----------------------------------------------------------------------------
