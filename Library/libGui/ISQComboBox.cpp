#include "ISQComboBox.h"
//-----------------------------------------------------------------------------
ISQComboBox::ISQComboBox(QWidget *parent) : QComboBox(parent)
{
	WheelScroll = false;
}
//-----------------------------------------------------------------------------
ISQComboBox::~ISQComboBox()
{

}
//-----------------------------------------------------------------------------
void ISQComboBox::SetWheelScroll(bool wheel_scroll)
{
	WheelScroll = wheel_scroll;
}
//-----------------------------------------------------------------------------
bool ISQComboBox::GetWheelScrool() const
{
	return WheelScroll;
}
//-----------------------------------------------------------------------------
void ISQComboBox::showPopup()
{
	QComboBox::showPopup();
	emit ShowPopup();
}
//-----------------------------------------------------------------------------
void ISQComboBox::hidePopup()
{
	QComboBox::hidePopup();
	emit HidePopup();
}
//-----------------------------------------------------------------------------
void ISQComboBox::wheelEvent(QWheelEvent *e)
{
	if (WheelScroll)
	{
		QComboBox::wheelEvent(e);
	}
	else
	{
		e->ignore();
	}
}
//-----------------------------------------------------------------------------
