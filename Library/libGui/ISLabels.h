#pragma once
#ifndef _ISLABELS_H_INCLUDED
#define _ISLABELS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISQLabel : public QLabel
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISLabelLink : public ISQLabel
{
	Q_OBJECT

public:
	ISLabelLink(const QString &Text, QWidget *parent = 0);
	ISLabelLink(QWidget *parent = 0);
	virtual ~ISLabelLink();

protected:
	void keyPressEvent(QKeyEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
};
//-----------------------------------------------------------------------------
#endif
