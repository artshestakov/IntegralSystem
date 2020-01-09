#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISIPAddressEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIPAddressEdit(QWidget *parent);
	virtual ~ISIPAddressEdit();
};
//-----------------------------------------------------------------------------
