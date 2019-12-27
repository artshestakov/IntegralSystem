#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISIPAddressEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIPAddressEdit(QWidget *parent);
	virtual ~ISIPAddressEdit();
};
//-----------------------------------------------------------------------------
