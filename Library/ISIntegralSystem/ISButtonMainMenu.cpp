#include "StdAfx.h"
#include "ISButtonMainMenu.h"
#include "EXDefines.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISButtonMainMenu::ISButtonMainMenu(QWidget *parent) : QToolButton(parent)
{
	IconDefault = BUFFER_ICONS("MainPanel.Menu");
	IconActive = BUFFER_ICONS("MainPanel.Menu.Active");

	setText(LOCALIZATION("MainMenu"));
	setFont(FONT_TAHOMA_10);
	setAutoRaise(true);
	setCheckable(true);
	setIcon(IconDefault);
	setIconSize(SIZE_24_24);
	setCursor(CURSOR_POINTING_HAND);
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	setStyleSheet(STYLE_SHEET("QToolButtonMainMenu"));
}
//-----------------------------------------------------------------------------
ISButtonMainMenu::~ISButtonMainMenu()
{

}
//-----------------------------------------------------------------------------
void ISButtonMainMenu::enterEvent(QEvent *e)
{
	QToolButton::enterEvent(e);
	setIcon(IconActive);
}
//-----------------------------------------------------------------------------
void ISButtonMainMenu::leaveEvent(QEvent *e)
{
	QToolButton::leaveEvent(e);
	setIcon(IconDefault);
}
//-----------------------------------------------------------------------------
