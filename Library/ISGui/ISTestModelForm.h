#pragma once
#ifndef _ISTESTMODELFORM_H_INCLUDED
#define _ISTESTMODELFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISTcpModel.h"
#include "ISBaseTableView.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
class ISTestModelForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTestModelForm(QWidget *parent = 0);
	virtual ~ISTestModelForm();

	void LoadData() override;

private:
	void SetSource(const QVariantMap &TcpAnswer);
	void SortingChanged(int LogicalIndex, Qt::SortOrder Order);

private:
	ISTcpModel *TcpModel;
	ISTcpQuery *qGetTableData;
	ISBaseTableView *TableView;

private:
	//QString SortingField;
	//Qt::SortOrder SortingOrder;
};
//-----------------------------------------------------------------------------
#endif
