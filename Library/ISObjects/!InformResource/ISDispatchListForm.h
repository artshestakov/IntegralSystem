#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISDispatchListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDispatchListForm(QWidget *parent = 0);
	virtual ~ISDispatchListForm();

private:
	void Send();
};
//-----------------------------------------------------------------------------
