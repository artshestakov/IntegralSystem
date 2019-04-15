#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISEMailSelectEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISEMailSelectEdit(QWidget *parent);
	virtual ~ISEMailSelectEdit();
};
//-----------------------------------------------------------------------------
