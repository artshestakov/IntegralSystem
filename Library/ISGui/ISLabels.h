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
	void mouseReleaseEvent(QMouseEvent *MouseEvent);
	void mouseDoubleClickEvent(QMouseEvent *MouseEvent);
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
	void keyPressEvent(QKeyEvent *KeyEvent);
	void enterEvent(QEvent *Event);
	void leaveEvent(QEvent *Event);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISLabelSelectionText : public ISQLabel
{
	Q_OBJECT

public:
	ISLabelSelectionText(const QString &Text, QWidget *parent = 0);
	ISLabelSelectionText(QWidget *parent = 0);
	~ISLabelSelectionText();

protected:
	void mouseReleaseEvent(QMouseEvent *MouseEvent) override;

private:
	void CopySelectedText();
};
//-----------------------------------------------------------------------------
#endif
