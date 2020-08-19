#include "ISSliderDuration.h"
//-----------------------------------------------------------------------------
ISSliderDuration::ISSliderDuration(QWidget *parent) : QSlider(Qt::Horizontal, parent)
{
	
}
//-----------------------------------------------------------------------------
ISSliderDuration::~ISSliderDuration()
{

}
//-----------------------------------------------------------------------------
void ISSliderDuration::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		if (orientation() == Qt::Horizontal)
		{
			setValue(minimum() + ((maximum() - minimum()) * e->x()) / width());
		}
		e->accept();
	}
	QSlider::mousePressEvent(e);
}
//-----------------------------------------------------------------------------
void ISSliderDuration::wheelEvent(QWheelEvent *WheelEvent)
{
	Q_UNUSED(WheelEvent);
	return;
}
//-----------------------------------------------------------------------------
