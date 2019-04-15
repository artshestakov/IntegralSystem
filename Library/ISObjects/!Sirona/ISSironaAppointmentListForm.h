#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISSironaAppointmentListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSironaAppointmentListForm(QWidget *parent = 0);
	virtual ~ISSironaAppointmentListForm();

protected:
	void AdmissionFinished(); //���� �������
	void FilterChange(QAbstractButton *AbstractButton); //������

private:
	QRadioButton *RadioAll;
	QRadioButton *RadioActual;
	QRadioButton *RadioFinished;
};
//-----------------------------------------------------------------------------
