#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISQDateTimeEdit : public QDateTimeEdit
{
	Q_OBJECT

public:
	ISQDateTimeEdit(QWidget *parent = 0);
	virtual ~ISQDateTimeEdit();

	void contextMenuEvent(QContextMenuEvent *e);
};
//-----------------------------------------------------------------------------
