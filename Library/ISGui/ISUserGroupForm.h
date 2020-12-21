#pragma once
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISUserGroupForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserGroupForm(int group_id, const QString &group_name);
	virtual ~ISUserGroupForm();

protected:
	void AfterShowEvent() override;

private:
	void CreateSubSystems();
	void CreateTables();
	void CreateSpecial();

private:
	void SubSystemClicked(const QVariant &value); //��������� ������� �� ����� ��� ����������
	void TableClicked(QAction *Action); //��������� ������� �� ����� ��� �������
	void SpecialClicked(const QVariant &value); //��������� ������� �� ����������� �����

private:
	QTabWidget *TabWidget;

private:
	int GroupID;
	QVariantList SubSystems;
};
//-----------------------------------------------------------------------------
