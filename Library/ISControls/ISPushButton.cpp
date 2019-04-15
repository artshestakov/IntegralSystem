#include "StdAfx.h"
#include "ISPushButton.h"
#include "EXDefines.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent) : QPushButton(Icon, Text, parent)
{
	setToolTip(ToolTip);
	setStyleSheet(STYLE_SHEET("ISPushButton"));
	setMinimumWidth(ISPUSHBUTTON_MINIMUM_WIDTH);
	setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, const QString &Text, QWidget *parent) : ISPushButton(Icon, Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QString &Text, QWidget *parent) : ISPushButton(QIcon(), Text, QString(), parent)
{
	
}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, QWidget *parent) : ISPushButton(Icon, QString(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(QWidget *parent) : ISPushButton(QIcon(), QString(), QString(), parent)
{
	
}
//-----------------------------------------------------------------------------
ISPushButton::~ISPushButton()
{

}
//-----------------------------------------------------------------------------
