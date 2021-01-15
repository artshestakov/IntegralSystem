#pragma once
#ifndef _ISLISTVIEWFORM_H_INCLUDED
#define _ISLISTVIEWFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISBaseTableView.h"
#include "ISViewModel.h"
//-----------------------------------------------------------------------------
class ISListViewForm : public ISInterfaceMetaForm
{
	Q_OBJECT

signals:
	void Updated();

public:
	ISListViewForm(const QString &query_name, QWidget *parent = 0);
	virtual ~ISListViewForm();

	void BindValue(const QString &ParameterName, const QVariant &Value);
	void LoadData() override;

private:
	ISBaseTableView *TableView;
	ISViewModel *ViewModel;

private:
	QString QueryName;
	QVariantMap Parameters;
};
//-----------------------------------------------------------------------------
#endif
