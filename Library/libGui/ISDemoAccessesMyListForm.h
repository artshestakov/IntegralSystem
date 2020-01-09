#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDemoAccessesMyListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDemoAccessesMyListForm(QWidget *parent = 0);
	virtual ~ISDemoAccessesMyListForm();
};
//-----------------------------------------------------------------------------
