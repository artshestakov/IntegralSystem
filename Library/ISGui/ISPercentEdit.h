#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISDoubleEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISPercentEdit : public ISDoubleEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPercentEdit(QWidget *parent);
	virtual ~ISPercentEdit();
};
//-----------------------------------------------------------------------------
