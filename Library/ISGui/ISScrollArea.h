#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISScrollArea : public QScrollArea
{
	Q_OBJECT

public:
	ISScrollArea(QWidget *parent = 0);
	virtual ~ISScrollArea();

	bool IsWheelTracking() const;
	void SetWheelTracking(bool Tracking);

protected:
	void wheelEvent(QWheelEvent *e);

private:
	QWidget *Widget;
	bool WheelTracking;
};
//-----------------------------------------------------------------------------
