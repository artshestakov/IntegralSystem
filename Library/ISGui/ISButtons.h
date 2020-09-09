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
class ISServiceButton : public ISPushButton
{
	Q_OBJECT

public:
	ISServiceButton(const QIcon &Icon, const QString &ToolTip, QWidget *parent = 0);
	ISServiceButton(const QIcon &Icon, QWidget *parent = 0);
	ISServiceButton(const QString &ToolTip, QWidget *parent = 0);
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
class ISButtonDialog : public QDialogButtonBox
{
	Q_OBJECT

signals:
	void Apply();
	void Close();

public:
	ISButtonDialog(QWidget *parent, const QString &ApplyText = QString(), const QString &CloseText = QString());
	virtual ~ISButtonDialog();

	void SetApplyIcon(const QIcon &ApplyIcon);
	void SetApplyEnabled(bool Enabled);
	void SetApplyVisible(bool Visible);
	void SetApplyCursor(const QCursor &Cursor);
	void SetApplyFocus();

	void SetCloseIcon(const QIcon &Icon);
	void SetCloseEnabled(bool Enabled);
	void SetCloseCursor(const QCursor &Cursor);
};
//-----------------------------------------------------------------------------
#endif
