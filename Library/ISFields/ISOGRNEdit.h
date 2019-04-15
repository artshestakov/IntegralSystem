#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISOGRNEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOGRNEdit(QWidget *parent);
	virtual ~ISOGRNEdit();
};
//-----------------------------------------------------------------------------
