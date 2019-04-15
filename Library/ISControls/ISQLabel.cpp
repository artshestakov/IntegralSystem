#include "StdAfx.h"
#include "ISQLabel.h"
//-----------------------------------------------------------------------------
ISQLabel::ISQLabel(const QString &Text, QWidget *parent) : QLabel(Text, parent)
{

}
//-----------------------------------------------------------------------------
ISQLabel::ISQLabel(QWidget *parent) : QLabel(parent)
{

}
//-----------------------------------------------------------------------------
ISQLabel::~ISQLabel()
{

}
//-----------------------------------------------------------------------------
void ISQLabel::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		emit Clicked();
	}
}
//-----------------------------------------------------------------------------
void ISQLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		emit DoubleClicked();
	}
}
//-----------------------------------------------------------------------------
