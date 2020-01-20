#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISTaskBaseListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskBaseListForm(QWidget *parent = 0);
	virtual ~ISTaskBaseListForm();

protected:
	void Edit() override;
	void DoubleClickedTable(const QModelIndex &ModelIndex) override;

	void TakeToWork();

private:
	QAction *ActionTakeToWork;
};
//-----------------------------------------------------------------------------
