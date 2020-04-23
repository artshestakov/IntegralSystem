#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISProtocolListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISProtocolListForm(QWidget *parent = 0);
	virtual ~ISProtocolListForm();

protected:
	void DoubleClickedTable(const QModelIndex &ModelIndex);
};
//-----------------------------------------------------------------------------
