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
	void PreviousDay(); //Предыдущий день
	void NextDay(); //Следующий день

	void AddWaitAdmission(); //Добавление ожидания приёма
	void LoadBranches(); //Загрузка списка филиалов
	void DateChanged(const QVariant &value); //Событие изменения даты
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
