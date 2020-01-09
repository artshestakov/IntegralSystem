#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboSearchBase.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboSearchString : public ISComboSearchBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchString(QWidget *parent = 0);
	virtual ~ISComboSearchString();
};
//-----------------------------------------------------------------------------
