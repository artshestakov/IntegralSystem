#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISOKPOEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOKPOEdit(QWidget *parent);
	virtual ~ISOKPOEdit();
};
//-----------------------------------------------------------------------------
