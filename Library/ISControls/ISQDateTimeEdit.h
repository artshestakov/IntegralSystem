#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISQDateTimeEdit : public QDateTimeEdit
{
	Q_OBJECT

public:
	ISQDateTimeEdit(QWidget *parent = 0);
	virtual ~ISQDateTimeEdit();

	void contextMenuEvent(QContextMenuEvent *e);
};
//-----------------------------------------------------------------------------
