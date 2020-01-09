#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISBIKEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBIKEdit(QWidget *parent);
	virtual ~ISBIKEdit();
};
//-----------------------------------------------------------------------------
