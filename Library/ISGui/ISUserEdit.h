#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISUserEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserEdit(QWidget *parent);
	virtual ~ISUserEdit();
};
//-----------------------------------------------------------------------------
