#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskMyListForm : public ISTaskBaseListForm
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
