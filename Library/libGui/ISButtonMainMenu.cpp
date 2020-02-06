#include "ISButtonMainMenu.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISButtonMainMenu::ISButtonMainMenu(QWidget *parent) : QToolButton(parent)
{
	IconDefault = BUFFER_ICONS("MainPanel.Menu");
	IconActive = BUFFER_ICONS("MainPanel.Menu.Active");

	setText(LANG("MainMenu"));
	setFont(DEFINES_GUI.FONT_TAHOMA_10);
	setAutoRaise(true);
	setCheckable(true);
	setIcon(IconDefault);
	setIconSize(DEFINES_GUI.SIZE_24_24);
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
