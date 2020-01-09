#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskAllListForm : public ISTaskBaseListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskAllListForm(QWidget *parent = 0);
	virtual ~ISTaskAllListForm();
};
//-----------------------------------------------------------------------------
