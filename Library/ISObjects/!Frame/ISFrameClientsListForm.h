#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISFrameClientsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFrameClientsListForm(QWidget *parent);
	virtual ~ISFrameClientsListForm();
};
//-----------------------------------------------------------------------------
