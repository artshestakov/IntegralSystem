#include "ISToolButton.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISToolButton::ISToolButton(QWidget *parent) : QToolButton(parent)
{
	setStyleSheet(STYLE_SHEET("ISPushButton"));
	setMinimumWidth(ISPUSHBUTTON_MINIMUM_WIDTH);
	setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
}
//-----------------------------------------------------------------------------
ISToolButton::~ISToolButton()
{

}
//-----------------------------------------------------------------------------
