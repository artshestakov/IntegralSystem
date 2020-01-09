#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboTableDateFormat : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDateFormat(QWidget *parent);
	virtual ~ISComboTableDateFormat();
};
//-----------------------------------------------------------------------------
