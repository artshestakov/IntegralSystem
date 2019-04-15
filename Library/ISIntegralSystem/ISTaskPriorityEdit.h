#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISRadioEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskPriorityEdit : public ISRadioEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskPriorityEdit(QWidget *parent);
	virtual ~ISTaskPriorityEdit();
};
//-----------------------------------------------------------------------------
