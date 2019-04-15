#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISICQEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISICQEdit(QWidget *parent);
	virtual ~ISICQEdit();
};
//-----------------------------------------------------------------------------
