#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISDoubleEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPercentEdit : public ISDoubleEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPercentEdit(QWidget *parent);
	virtual ~ISPercentEdit();
};
//-----------------------------------------------------------------------------
