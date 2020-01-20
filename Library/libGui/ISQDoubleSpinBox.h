#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
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
