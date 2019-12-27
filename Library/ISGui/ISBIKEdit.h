#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISBIKEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBIKEdit(QWidget *parent);
	virtual ~ISBIKEdit();
};
//-----------------------------------------------------------------------------
