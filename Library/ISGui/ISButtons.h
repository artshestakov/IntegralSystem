#pragma once
#ifndef _ISBUTTONS_H_INCLUDED
#define _ISBUTTONS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISPushButton : public QPushButton
{
	Q_OBJECT

public:
	ISPushButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent = 0);
	ISPushButton(const QIcon &Icon, const QString &Text, QWidget *parent = 0);
	ISPushButton(const QString &Text, QWidget *parent = 0);
	ISPushButton(const QIcon &Icon, QWidget *parent = 0);
	ISPushButton(QWidget *parent = 0);
	virtual ~ISPushButton();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISButtonClear : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClear(QWidget *parent = 0);
	virtual ~ISButtonClear();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISButtonClose : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClose(QWidget *parent = 0);
	virtual ~ISButtonClose();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISServiceButton : public ISPushButton
{
	Q_OBJECT

public:
	ISServiceButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent = 0);
	ISServiceButton(const QIcon &Icon, const QString &Text, QWidget *parent = 0);
	ISServiceButton(const QString &Text, QWidget *parent = 0);
	ISServiceButton(QWidget *parent = 0);
	virtual ~ISServiceButton();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISToolButton : public QToolButton
{
	Q_OBJECT

public:
	ISToolButton(QWidget *parent = 0);
	virtual ~ISToolButton();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISButtonMainMenu : public QToolButton
{
	Q_OBJECT

public:
	ISButtonMainMenu(QWidget *parent = 0);
	virtual ~ISButtonMainMenu();

protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);

private:
	QIcon IconDefault;
	QIcon IconActive;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISButtonFile : public ISPushButton
{
	Q_OBJECT

signals:
	void DragAndDropFile(const QString &);

public:
	ISButtonFile(QWidget *parent = 0);
	virtual ~ISButtonFile();

protected:
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISParagraphButton : public QWidget
{
	Q_OBJECT

		signals :
	void Clicked();

public:
	ISParagraphButton(ISMetaParagraph *MetaParagraph, QWidget *parent = 0);
	virtual ~ISParagraphButton();

	void SetText(const QString &Text);
	void SetToolTip(const QString &ToolTip);
	void SetCursor(const QCursor &Cursor);
	void SetVisibleLine(bool Visible);

private:
	QToolButton *ToolButton;

	QWidget *LineLeft;
	QLabel *LineCenter;
	QWidget *LineRight;
};
//-----------------------------------------------------------------------------
#endif
