#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISEMailSelectEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISEMailSelectEdit(QWidget *parent);
	virtual ~ISEMailSelectEdit();
};
//-----------------------------------------------------------------------------
