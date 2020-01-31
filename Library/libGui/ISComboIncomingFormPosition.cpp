#include "StdAfx.h"
#include "ISComboIncomingFormPosition.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboIncomingFormPosition::ISComboIncomingFormPosition(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.IncomingForm.Position.LeftUp"), "LeftUp");
	AddItem(LANG("Settings.IncomingForm.Position.LeftBottom"), "LeftBottom");
	AddItem(LANG("Settings.IncomingForm.Position.RightUp"), "RightUp");
	AddItem(LANG("Settings.IncomingForm.Position.RightBottom"), "RightBottom");
	AddItem(LANG("Settings.IncomingForm.Position.Center"), "Center");
}
//-----------------------------------------------------------------------------
ISComboIncomingFormPosition::~ISComboIncomingFormPosition()
{

}
//-----------------------------------------------------------------------------
