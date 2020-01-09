#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboIncomingFormPosition : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboIncomingFormPosition(QWidget *parent);
	virtual ~ISComboIncomingFormPosition();
};
//-----------------------------------------------------------------------------
