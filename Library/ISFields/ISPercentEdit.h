#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISDoubleEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISPercentEdit : public ISDoubleEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPercentEdit(QWidget *parent);
	virtual ~ISPercentEdit();
};
//-----------------------------------------------------------------------------
