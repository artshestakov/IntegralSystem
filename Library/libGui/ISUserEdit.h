#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISUserEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserEdit(QWidget *parent);
	virtual ~ISUserEdit();
};
//-----------------------------------------------------------------------------
