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
	ISExportForm(PMetaTable *meta_table);
	virtual ~ISExportForm();

	ISNamespace::ExportType GetSelectedType();
	QString GetSelectTypeName() const;
	ISVectorString GetSelectedFields() const;
	bool GetHeader() const;

protected:
	void CreateTabSettings();
	void CreateTabFields();
	void Select();

	void CreateFieldItem(PMetaField *MetaField);
	void FieldsPositionChanged();
	void ItemDoubleClicked(QListWidgetItem *item);

	void EnterClicked();

private:
	ISNamespace::ExportType SelectedType;
	PMetaTable *MetaTable;
	ISVectorString SelectedFields;
	ISComboEdit *ComboBoxType;
	QTabWidget *TabWidget;
	ISButtonDialog *ButtonPanel;
	ISListWidget *ListFields;

	ISCheckEdit *CheckHeader;
};
//-----------------------------------------------------------------------------
