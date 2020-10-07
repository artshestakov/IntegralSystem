#pragma once
#ifndef _ISLISTVIEWFORM_H_INCLUDED
#define _ISLISTVIEWFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISBaseTableView.h"
//-----------------------------------------------------------------------------
class ISListViewForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	ISListViewForm(const QString &query_name, QWidget *parent = 0);
	virtual ~ISListViewForm();

	void BindValue(const QString &ParameterName, const QVariant &Value);
	void LoadData() override;

private:
	ISBaseTableView *TableView;
	QSqlQueryModel *SqlModel;

private:
	QString QueryName;
	QString SqlText;
	ISStringToVariantMap Paramters;
};
//-----------------------------------------------------------------------------
#endif
