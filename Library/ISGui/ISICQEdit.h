#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISICQEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISICQEdit(QWidget *parent);
	virtual ~ISICQEdit();
};
//-----------------------------------------------------------------------------
