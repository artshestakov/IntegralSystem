#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboTableDateFormat : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDateFormat(QWidget *parent);
	virtual ~ISComboTableDateFormat();
};
//-----------------------------------------------------------------------------
