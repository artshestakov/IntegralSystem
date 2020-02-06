#include "ISButtonClear.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISButtonClear::ISButtonClear(QWidget *parent) : ISPushButton(parent)
{
	setFixedSize(DEFINES_GUI.SIZE_22_22);
	setCursor(CURSOR_POINTING_HAND);
	setToolTip(LANG("Clear.Field"));
	setFocusPolicy(Qt::NoFocus);
	setIcon(BUFFER_ICONS("ButtonClear"));
	setFlat(true);
}
//-----------------------------------------------------------------------------
ISButtonClear::~ISButtonClear()
{

}
//-----------------------------------------------------------------------------
