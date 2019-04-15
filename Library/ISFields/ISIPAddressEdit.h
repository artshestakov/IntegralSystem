#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISIPAddressEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIPAddressEdit(QWidget *parent);
	virtual ~ISIPAddressEdit();
};
//-----------------------------------------------------------------------------
