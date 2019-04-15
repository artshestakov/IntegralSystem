#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISQTextEdit : public QTextEdit
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
