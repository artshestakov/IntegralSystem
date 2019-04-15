#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISPatriotVisitListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotVisitListForm(QWidget *parent = 0);
	virtual ~ISPatriotVisitListForm();

protected:
	void CancelVisit();
};
//-----------------------------------------------------------------------------
