#pragma once
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISUserGroupForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserGroupForm(int group_id, const QString &group_name);
	virtual ~ISUserGroupForm();

protected:
	void CreateSubSystems();
	void CreateTables();
	void CreateSpecial();

	void SubSystemClicked(const QVariant &value); //��������� ������� �� ����� ��� ����������
	void TableClicked(QAction *Action); //��������� ������� �� ����� ��� �������
	void SpecialClicked(const QVariant &value); //��������� ������� �� ����������� �����

private:
	int GroupID;
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
