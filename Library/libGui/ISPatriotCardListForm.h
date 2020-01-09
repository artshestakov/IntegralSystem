#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISPatriotCardListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotCardListForm(QWidget *parent = 0);
	virtual ~ISPatriotCardListForm();

protected:
	void Deactivate();
};
//-----------------------------------------------------------------------------
