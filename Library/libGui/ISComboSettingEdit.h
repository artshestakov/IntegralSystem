#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboSettingEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSettingEdit(QWidget *parent);
	virtual ~ISComboSettingEdit();
};
//-----------------------------------------------------------------------------
