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
	void Search(const QVariantList &VariantList);

public:
	ISSearchForm(PMetaTable *meta_table, QWidget *parent = 0);
	virtual ~ISSearchForm();

protected:
	void closeEvent(QCloseEvent *e) override;
	void EscapeClicked() override;
	void EnterClicked() override;

private:
	void AddField(PMetaField *MetaField, QTreeWidgetItem *ParentItem = nullptr);
	void AddClicked();
	void DeleteClicked();

	void Clear();
	void Search();
	bool ExistField(const QVariantList &VariantList, const QString &FieldName);
	
private:
	PMetaTable *MetaTable;
	
	QTreeWidget *TreeWidget;
	std::map<QObject*, QTreeWidgetItem*> Map;
};
//-----------------------------------------------------------------------------
