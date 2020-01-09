#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISICQEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISICQEdit(QWidget *parent);
	virtual ~ISICQEdit();
};
//-----------------------------------------------------------------------------
