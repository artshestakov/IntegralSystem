#pragma once
#ifndef _ISEDITSQT_H_INCLUDED
#define _ISEDITSQT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISCalendarPopup.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISQComboBox : public QComboBox
{
	Q_OBJECT

signals:
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
class ISQLineEdit : public QLineEdit
{
	Q_OBJECT

signals:
	void ClearClicked();

public:
	ISQLineEdit(QWidget *parent = 0);
	virtual ~ISQLineEdit();

	void SetEnabled(bool Enabled);
	void SetIcon(const QIcon &icon);
	void SetVisibleClear(bool visible);
	void AddAction(QAction *Action, QLineEdit::ActionPosition Position);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void paintEvent(QPaintEvent *e);

private:
	QPixmap PixmapEnabled;
	QPixmap PixmapCurrent;
	QAction *ActionClear;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISQDateEdit : public ISQLineEdit
{
	Q_OBJECT

signals:
	void DateChanged();

public:
	ISQDateEdit(QWidget *parent = 0);
	virtual ~ISQDateEdit();

	void contextMenuEvent(QContextMenuEvent *e);

	QDate GetDate() const;
	void SetDate(const QDate &Date);
	void Clear();
	void SetEnabled(bool Enabled);
	void SetReadOnly(bool ReadOnly);

	void SetRange(const QDate &Minimum, const QDate &Maximum);
	void SetMinimumDate(const QDate &Date);
	void SetMaximumDate(const QDate &Date);

protected:
	void showEvent(QShowEvent *ShowEvent);
	void resizeEvent(QResizeEvent *ResizeEvent);

private:
	void ShowCalendar();
	void HideCalendar();
	void SelectionChanged();

private:
	ISServiceButton *ButtonCalendar;
	ISCalendarPopup *CalendarWidget;
	bool IsShow;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISQTimeEdit : public QWidget
{
	Q_OBJECT

signals:
	void TimeChanged();

public:
	ISQTimeEdit(QWidget *parent = 0);
	virtual ~ISQTimeEdit();

	QTime GetTime() const;
	void SetTime(const QTime &Time);
	void Clear();
	void SetReadOnly(bool ReadOnly);

private:
	ISQLineEdit *EditHour;
	ISQLineEdit *EditMinute;
	ISQLineEdit *EditSecond;
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
