#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskOverdueListForm : public ISTaskBaseListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskOverdueListForm(QWidget *parent = 0);
	virtual ~ISTaskOverdueListForm();
};
//-----------------------------------------------------------------------------
