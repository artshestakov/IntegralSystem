#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISComboSearchBase.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISComboSearchNumber : public ISComboSearchBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchNumber(QWidget *parent = 0);
	virtual ~ISComboSearchNumber();
};
//-----------------------------------------------------------------------------
