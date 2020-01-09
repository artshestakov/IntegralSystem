#include "StdAfx.h"
#include "ISComboIncomingFormPosition.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboIncomingFormPosition::ISComboIncomingFormPosition(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LOCALIZATION("Settings.IncomingForm.Position.LeftUp"), "LeftUp");
	AddItem(LOCALIZATION("Settings.IncomingForm.Position.LeftBottom"), "LeftBottom");
	AddItem(LOCALIZATION("Settings.IncomingForm.Position.RightUp"), "RightUp");
	AddItem(LOCALIZATION("Settings.IncomingForm.Position.RightBottom"), "RightBottom");
	AddItem(LOCALIZATION("Settings.IncomingForm.Position.Center"), "Center");
}
//-----------------------------------------------------------------------------
ISComboIncomingFormPosition::~ISComboIncomingFormPosition()
{

}
//-----------------------------------------------------------------------------
