#include "StdAfx.h"
#include "ISButtonClose.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISButtonClose::ISButtonClose(QWidget *parent) : ISPushButton(parent)
{
	setText(LOCALIZATION("Close"));
	setIcon(BUFFER_ICONS("Close"));
}
//-----------------------------------------------------------------------------
ISButtonClose::~ISButtonClose()
{

}
//-----------------------------------------------------------------------------
