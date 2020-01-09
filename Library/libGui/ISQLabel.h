#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISQLabel : public QLabel
{
	Q_OBJECT

signals:
	void Clicked();
	void DoubleClicked();

public:
	ISQLabel(const QString &Text, QWidget *parent = 0);
	ISQLabel(QWidget *parent = 0);
	virtual ~ISQLabel();

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
};
//-----------------------------------------------------------------------------
