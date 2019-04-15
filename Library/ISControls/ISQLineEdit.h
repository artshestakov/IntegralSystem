#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISQLineEdit : public QLineEdit
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
