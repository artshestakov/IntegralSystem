#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboSettingEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSettingEdit(QWidget *parent);
	virtual ~ISComboSettingEdit();
};
//-----------------------------------------------------------------------------
