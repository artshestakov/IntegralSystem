#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "PMetaClass.h"
#include "ISNamespace.h"
#include "ISListWidget.h"
#include "ISFieldEdits.h"
#include "ISTcpModels.h"
//-----------------------------------------------------------------------------
class ISExportDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISExportDialog(ISTcpModel *TcpModel);
	virtual ~ISExportDialog();

	ISNamespace::ExportType GetSelectedType();
	QString GetSelectTypeName() const;
	ISVectorUInt GetSelectedFields() const;
	bool GetHeader() const;

private:
	void CreateTabSettings();
	void CreateTabFields(ISTcpModel *TcpModel);
	void Select();

	void CreateFieldItem(const ISModelField &ModelField);
	void FieldsPositionChanged();
	void ItemDoubleClicked(QListWidgetItem *ListWidgetItem);

	void EnterClicked();
	void TypeChanged(const QVariant &Value);

private:
	ISNamespace::ExportType SelectedType;
	ISVectorUInt SelectedFields;
	ISComboEdit *ComboBoxType;
	QTabWidget *TabWidget;
	ISButtonDialog *ButtonDialog;
	ISListWidget *ListFields;

	ISCheckEdit *CheckHeader;
};
//-----------------------------------------------------------------------------
