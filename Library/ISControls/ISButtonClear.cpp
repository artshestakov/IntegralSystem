#include "StdAfx.h"
#include "ISButtonClear.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISButtonClear::ISButtonClear(QWidget *parent) : ISPushButton(parent)
{
	setFixedSize(SIZE_22_22);
	setCursor(CURSOR_POINTING_HAND);
	setToolTip(LOCALIZATION("Clear.Field"));
	setFocusPolicy(Qt::NoFocus);
	setIcon(BUFFER_ICONS("ButtonClear"));
	setFlat(true);
}
//-----------------------------------------------------------------------------
ISButtonClear::~ISButtonClear()
{

}
//-----------------------------------------------------------------------------
