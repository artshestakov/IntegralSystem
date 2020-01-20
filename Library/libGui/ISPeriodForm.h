#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISNamespace.h"
#include "ISRangeDateEdit.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
class ISPeriodForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISPeriodForm(QWidget *parent = 0);
	virtual ~ISPeriodForm();

	ISNamespace::PeriodType GetPeriodType() const;
	ISRangeStruct GetRange() const;
	void SetRange(ISNamespace::PeriodType PeriodType, const ISRangeStruct &Range);

protected:
	void RadioChanged();
	void PeriodInstall();

private:
	QButtonGroup *ButtonGroup;
	QRadioButton *RadioButtonCreate;
	QRadioButton *RadioButtonEdit;
	ISRangeDateEdit *RangeDateEdit;
	ISButtonDialog *ButtonDialog;
};
//-----------------------------------------------------------------------------
