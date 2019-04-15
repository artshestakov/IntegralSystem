#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISComboEdit.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISComboSearchBase : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchBase(QWidget *parent = 0);
	virtual ~ISComboSearchBase();

	ISNamespace::SearchOperatorType GetOperator();
};
//-----------------------------------------------------------------------------
