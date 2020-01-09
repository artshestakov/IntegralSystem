#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDatabaseEdit : public ISComboEdit
{
	Q_OBJECT

public:
	ISDatabaseEdit(QWidget *parent = 0);
	virtual ~ISDatabaseEdit();
};
//-----------------------------------------------------------------------------
