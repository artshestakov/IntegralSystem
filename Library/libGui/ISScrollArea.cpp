#include "ISScrollArea.h"
#include "EXDefines.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISScrollArea::ISScrollArea(QWidget *parent) : QScrollArea(parent)
{
	WheelTracking = true;

	Widget = new QWidget(this);
	Widget->setObjectName("WidgetScrollArea");
	Widget->setStyleSheet(STYLE_SHEET("ISScrollArea"));
	setWidget(Widget);

	setFrameShape(QFrame::NoFrame);
	setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	verticalScrollBar()->setCursor(CURSOR_POINTING_HAND);
	horizontalScrollBar()->setCursor(CURSOR_POINTING_HAND);
	setWidgetResizable(true);
}
//-----------------------------------------------------------------------------
ISScrollArea::~ISScrollArea()
{

}
//-----------------------------------------------------------------------------
bool ISScrollArea::IsWheelTracking() const
{
	return WheelTracking;
}
//-----------------------------------------------------------------------------
void ISScrollArea::SetWheelTracking(bool Tracking)
{
	WheelTracking = Tracking;
}
//-----------------------------------------------------------------------------
void ISScrollArea::wheelEvent(QWheelEvent *e)
{
	if (WheelTracking)
	{
		QScrollArea::wheelEvent(e);
	}
	else
	{
		e->ignore();
	}
}
//-----------------------------------------------------------------------------
