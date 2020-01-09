#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISQSpinBox : public QSpinBox
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
