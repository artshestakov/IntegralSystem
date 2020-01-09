#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISFrameClientsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFrameClientsListForm(QWidget *parent);
	virtual ~ISFrameClientsListForm();
};
//-----------------------------------------------------------------------------
