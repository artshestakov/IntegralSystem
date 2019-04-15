#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboIncomingFormPosition : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboIncomingFormPosition(QWidget *parent);
	virtual ~ISComboIncomingFormPosition();
};
//-----------------------------------------------------------------------------
