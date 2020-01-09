#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskFromListForm : public ISTaskBaseListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskFromListForm(QWidget *parent = 0);
	virtual ~ISTaskFromListForm();
};
//-----------------------------------------------------------------------------
