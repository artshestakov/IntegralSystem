#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDemoAccessesMyCreatedListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDemoAccessesMyCreatedListForm(QWidget *parent = 0);
	virtual ~ISDemoAccessesMyCreatedListForm();
};
//-----------------------------------------------------------------------------
