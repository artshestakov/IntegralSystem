#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISOGRNEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOGRNEdit(QWidget *parent);
	virtual ~ISOGRNEdit();
};
//-----------------------------------------------------------------------------
