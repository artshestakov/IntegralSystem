#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISSliderDuration : public QSlider
{
	Q_OBJECT

public:
	ISSliderDuration(QWidget *parent = 0);
	virtual ~ISSliderDuration();

protected:
	void mousePressEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
};
//-----------------------------------------------------------------------------
