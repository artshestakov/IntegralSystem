#include "StdAfx.h"
#include "ISServiceButton.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent) : ISPushButton(Icon, Text, ToolTip, parent)
{
	setFixedSize(SIZE_22_22);
	setCursor(CURSOR_POINTING_HAND);
}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QIcon &Icon, const QString &Text, QWidget *parent) : ISServiceButton(Icon, Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QString &Text, QWidget *parent) : ISServiceButton(QIcon(), Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(QWidget *parent) : ISServiceButton(QIcon(), QString(), QString(), parent)
{
	
}
//-----------------------------------------------------------------------------
ISServiceButton::~ISServiceButton()
{

}
//-----------------------------------------------------------------------------
