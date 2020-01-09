#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskTodayListForm : public ISTaskBaseListForm
{
	Q_OBJECT
		
public:
	Q_INVOKABLE ISTaskTodayListForm(QWidget *parent = 0);
	virtual ~ISTaskTodayListForm();
};
//-----------------------------------------------------------------------------
