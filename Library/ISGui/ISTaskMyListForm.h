#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISTaskMyListForm : public ISTaskBaseListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskMyListForm(QWidget *parent = 0);
	virtual ~ISTaskMyListForm();

protected:
	QAction* CreateActionFilter(const QString &StatusName, const ISUuid &StatusUID);
	void FilterChanged();

private:
	QAction *ActionFilter;
};
//-----------------------------------------------------------------------------
