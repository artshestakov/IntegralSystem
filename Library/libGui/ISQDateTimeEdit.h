#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
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
