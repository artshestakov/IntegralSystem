#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISRadioEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskPriorityEdit : public ISRadioEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskPriorityEdit(QWidget *parent);
	virtual ~ISTaskPriorityEdit();
};
//-----------------------------------------------------------------------------
