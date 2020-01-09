#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISOGRNEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOGRNEdit(QWidget *parent);
	virtual ~ISOGRNEdit();
};
//-----------------------------------------------------------------------------
