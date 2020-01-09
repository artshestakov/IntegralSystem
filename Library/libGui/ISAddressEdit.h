#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISAddressEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAddressEdit(QWidget *parent = 0);
	virtual ~ISAddressEdit();

protected:
	void ListFIAS();
};
//-----------------------------------------------------------------------------
