#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "PMetaClass.h"
#include "ISButtons.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
class ISSearchForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void StartSearch(const QString &SearchString, const QVariantMap &VariantMap);

public:
	ISSearchForm(PMetaTable *meta_table, QWidget *parent = 0);
	virtual ~ISSearchForm();

protected:
	void closeEvent(QCloseEvent *e) override;
	void EscapeClicked() override;
	void EnterClicked() override;

private:
	void AddField(PMetaField *MetaField, int Index = -1);
	void AddClicked();
	void DeleteClicked();

	void Search();

	
private:
	PMetaTable *MetaTable;
	
	QTreeWidget *TreeWidget;
	ISPushButton *ButtonSearch;
	ISPushButton *ButtonHide;
	std::vector<QWidget*> VectorLabels;
	std::vector<QWidget*> VectorOperators;
};
//-----------------------------------------------------------------------------
