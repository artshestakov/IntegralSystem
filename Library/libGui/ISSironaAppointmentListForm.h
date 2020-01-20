#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISSironaAppointmentListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSironaAppointmentListForm(QWidget *parent = 0);
	virtual ~ISSironaAppointmentListForm();

protected:
	void AdmissionFinished(); //Приём окончен
	void FilterChange(QAbstractButton *AbstractButton); //Фильтр

private:
	QRadioButton *RadioAll;
	QRadioButton *RadioActual;
	QRadioButton *RadioFinished;
};
//-----------------------------------------------------------------------------
