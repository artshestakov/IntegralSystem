#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISPatriot.h"
#include "ISInterfaceMetaForm.h"
#include "ISRangeDateEdit.h"
#include "ISComboEdit.h"
#include "ISListViewWidget.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISPatriotTrainerReportForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotTrainerReportForm(QWidget *parent = 0);
	virtual ~ISPatriotTrainerReportForm();

	void LoadData() override;

private:
	ISRangeDateEdit *RangeDateEdit;
	ISComboEdit *EditTrainer;
	QLabel *LabelReport;
	ISListViewWidget *ListViewWidget;
};
//-----------------------------------------------------------------------------
