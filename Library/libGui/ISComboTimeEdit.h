#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboTimeEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTimeEdit(QWidget *parent);
	virtual ~ISComboTimeEdit();
};
//-----------------------------------------------------------------------------
