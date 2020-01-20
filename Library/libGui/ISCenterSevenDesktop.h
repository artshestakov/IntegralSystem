#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISDateEdit.h"
#include "ISCenterSevenAdmissionListForm.h"
//-----------------------------------------------------------------------------
class ISCenterSevenDesktop : public QWidget
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenDesktop(QWidget *parent = 0);
	virtual ~ISCenterSevenDesktop();

protected:
	void PreviousDay(); //���������� ����
	void NextDay(); //��������� ����

	void AddWaitAdmission(); //���������� �������� �����
	void LoadBranches(); //�������� ������ ��������
	void DateChanged(const QVariant &value); //������� ��������� ����
	void BranchChanged();
	void AllDoctorClicked();
	void DoctorClicked();
	void CreateAdmissionObjectForm(QWidget *ObjectFormBase);

private:
	QVBoxLayout *MainLayout;
	QLabel *LabelDayWeek;
	ISDateEdit *DateEdit;
	ISComboEdit *ComboBranch;
	QToolBar *ToolBar;
	QAction *ActionDoctors;
	QActionGroup *ActionGroup;
	ISCenterSevenAdmissionListForm *AdmissionListForm;
	int CurrentDoctorID;
};
//-----------------------------------------------------------------------------
