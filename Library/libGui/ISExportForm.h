#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "PMetaClass.h"
#include "ISComboEdit.h"
#include "ISNamespace.h"
#include "ISButtonDialog.h"
#include "ISCheckEdit.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISExportForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISExportForm(PMetaClassTable *meta_table, QWidget *parent = 0);
	virtual ~ISExportForm();

	ISNamespace::ExportType GetSelectedType();
	QString GetSelectTypeName() const;
	QList<QString> GetSelectedFields() const;
	bool GetHeader() const;

protected:
	void CreateTabSettings();
	void CreateTabFields();
	void Select();

	void CreateFieldItem(PMetaClassField *MetaField);
	void FieldsPositionChanged();
	void ItemDoubleClicked(QListWidgetItem *item);

	void EnterClicked();

private:
	ISNamespace::ExportType SelectedType;
	PMetaClassTable *MetaTable;
	QList<QString> SelectedFields;
	ISComboEdit *ComboBoxType;
	QTabWidget *TabWidget;
	ISButtonDialog *ButtonPanel;
	ISListWidget *ListFields;

	ISCheckEdit *CheckHeader;
};
//-----------------------------------------------------------------------------
