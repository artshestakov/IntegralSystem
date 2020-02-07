#pragma once
#ifndef _ISEDITS_H_INCLUDED
#define _ISEDITS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISQComboBox : public QComboBox
{
	Q_OBJECT

		signals :
	void ShowPopup();
	void HidePopup();

public:
	ISQComboBox(QWidget *parent = 0);
	virtual ~ISQComboBox();

	void SetWheelScroll(bool wheel_scroll);
	bool GetWheelScrool() const;

	void showPopup();
	void hidePopup();
	void wheelEvent(QWheelEvent *e);

protected:
	bool WheelScroll;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISQDateTimeEdit : public QDateTimeEdit
{
	Q_OBJECT

public:
	ISQDateTimeEdit(QWidget *parent = 0);
	virtual ~ISQDateTimeEdit();

	void contextMenuEvent(QContextMenuEvent *e);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISQDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

signals:
	void ClearClicked();

public:
	ISQDoubleSpinBox(QWidget *parent = 0);
	virtual ~ISQDoubleSpinBox();

	void SetVisibleClear(bool Visible);
	QValidator::State validate(QString &Input, int &Pos) const;

protected:
	void contextMenuEvent(QContextMenuEvent *e);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISQLineEdit : public QLineEdit
{
	Q_OBJECT

signals:
	void FocusInSignal();
	void FocusOutSignal();
	void MouseDoubleClicked();
	void WheelEventSignal(QWheelEvent *WheelEvent);
	void KeyPressEventSignal(QKeyEvent *KeyEvent);
	void MousePressEvent();
	void ClearClicked();

public:
	ISQLineEdit(QWidget *parent = 0);
	virtual ~ISQLineEdit();

	void SetIcon(const QIcon &icon);
	void SetMenuSelected(bool menu);
	void SetVisibleClear(bool visible);
	void AddAction(QAction *Action, QLineEdit::ActionPosition Position);

protected:
	void ContextMenuCopy();
	virtual void focusInEvent(QFocusEvent *e);
	virtual void focusOutEvent(QFocusEvent *e);
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void paintEvent(QPaintEvent *e);

private:
	QIcon Icon;
	QMenu *MenuCopy;
	QAction *ActionClear;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISQSpinBox : public QSpinBox
{
	Q_OBJECT

signals:
	void ClearClicked();

public:
	ISQSpinBox(QWidget *parent = 0);
	virtual ~ISQSpinBox();

	void SetCursorPosition(int Position);
	void SetVisibleClear(bool Visible);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *e);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISQTextEdit : public QTextEdit
{
	Q_OBJECT

signals:
	void KeyPressEnter();

public:
	ISQTextEdit(QWidget *parent = 0);
	virtual ~ISQTextEdit();

	void SetExecuteEnter(bool Enable);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);

private:
	bool Wheel;
	bool ExecEnter;
};
//-----------------------------------------------------------------------------
#endif
