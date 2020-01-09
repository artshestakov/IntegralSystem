#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboTableSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableSelectionBehavior(QWidget *parent);
	virtual ~ISComboTableSelectionBehavior();
};
//-----------------------------------------------------------------------------
