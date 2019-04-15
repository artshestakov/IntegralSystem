#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISComboSearchBase.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISComboSearchString : public ISComboSearchBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchString(QWidget *parent = 0);
	virtual ~ISComboSearchString();
};
//-----------------------------------------------------------------------------
