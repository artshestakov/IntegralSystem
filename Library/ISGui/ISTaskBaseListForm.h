#pragma once
#ifndef _ISTASKBASELISTFORM_H_INCLUDED
#define _ISTASKBASELISTFORM_H_INCLUDED
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
#endif
