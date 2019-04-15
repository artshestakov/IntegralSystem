#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskDescriptionEdit : public ISTextEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskDescriptionEdit(QWidget *parent = 0);
	virtual ~ISTaskDescriptionEdit();
};
//-----------------------------------------------------------------------------
