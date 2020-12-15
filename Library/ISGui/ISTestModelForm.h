#pragma once
#ifndef _ISTESTMODELFORM_H_INCLUDED
#define _ISTESTMODELFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISTcpModel.h"
#include "ISBaseTableView.h"
//-----------------------------------------------------------------------------
class ISTestModelForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTestModelForm(QWidget *parent = 0);
	virtual ~ISTestModelForm();

	void LoadData() override;

private:
	ISTcpModel *TcpModel;
	ISBaseTableView *TableView;
};
//-----------------------------------------------------------------------------
#endif
