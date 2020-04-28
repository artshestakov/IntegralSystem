#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISCenterSevenAdmissionListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	ISCenterSevenAdmissionListForm(QWidget *parent = 0);
	virtual ~ISCenterSevenAdmissionListForm();
	
	void SetEnabledActions(bool Enabled);
	void SetBranch(int branch_id);
	void SetDate(const QDate &date);
	void SetDoctor(int doctor_id);

	void Update() override;

protected:
	void StatusChange();

private:
	int BranchID;
	int DoctorID;
	QDate Date;
};
//-----------------------------------------------------------------------------