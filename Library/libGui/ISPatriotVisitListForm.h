#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPatriotVisitListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotVisitListForm(QWidget *parent = 0);
	virtual ~ISPatriotVisitListForm();

protected:
	void CancelVisit();
};
//-----------------------------------------------------------------------------
