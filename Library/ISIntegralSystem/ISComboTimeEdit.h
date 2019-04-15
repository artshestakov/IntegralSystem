#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboTimeEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTimeEdit(QWidget *parent);
	virtual ~ISComboTimeEdit();
};
//-----------------------------------------------------------------------------
