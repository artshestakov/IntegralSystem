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
	void CreateSubSystems(const QVariantList &Systems);
	void CreateTables();
	void CreateSpecial(const QVariantList &SpecialRights);

private:
	void SubSystemClicked(const QVariant &value); //��������� ������� �� ����� ��� ����������
	void TableClicked(QAction *Action); //��������� ������� �� ����� ��� �������
	void SpecialClicked(const QVariant &value); //��������� ������� �� ����������� �����

private:
	QTabWidget *TabWidget;

private:
	int GroupID;
	QVariantMap Tables;
};
//-----------------------------------------------------------------------------
