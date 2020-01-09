#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISProtocolListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISProtocolListForm(QWidget *parent = 0);
	virtual ~ISProtocolListForm();

protected:
	void DoubleClickedTable(const QModelIndex &ModelIndex);
};
//-----------------------------------------------------------------------------
