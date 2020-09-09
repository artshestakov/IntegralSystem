#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISNamespace.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISPeriodForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISPeriodForm();
	virtual ~ISPeriodForm();

	ISNamespace::PeriodType GetPeriodType() const;
	QDate GetBegin() const;
	QDate GetEnd() const;
	void SetRange(ISNamespace::PeriodType PeriodType, const QDate &DateBegin, const QDate &DateEnd);

protected:
	void RadioChanged();
	void PeriodSelected();
	void PeriodInstall();

private:
	QButtonGroup *ButtonGroup;
	QRadioButton *RadioButtonCreate;
	QRadioButton *RadioButtonEdit;
	ISDateEdit *EditBegin;
	ISDateEdit *EditEnd;
	ISButtonDialog *ButtonDialog;
};
//-----------------------------------------------------------------------------
