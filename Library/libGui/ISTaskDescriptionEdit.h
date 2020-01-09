#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskDescriptionEdit : public ISTextEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskDescriptionEdit(QWidget *parent = 0);
	virtual ~ISTaskDescriptionEdit();
};
//-----------------------------------------------------------------------------
