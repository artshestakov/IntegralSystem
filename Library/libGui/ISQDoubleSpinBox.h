#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISQDoubleSpinBox : public QDoubleSpinBox
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
