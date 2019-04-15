#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboTableSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableSelectionBehavior(QWidget *parent);
	virtual ~ISComboTableSelectionBehavior();
};
//-----------------------------------------------------------------------------
