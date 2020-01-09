#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISOKPOEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOKPOEdit(QWidget *parent);
	virtual ~ISOKPOEdit();
};
//-----------------------------------------------------------------------------
