#pragma once
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISUserGroupRightDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserGroupRightDialog(int group_id, const QString &group_name);
	virtual ~ISUserGroupRightDialog();

protected:
	void AfterShowEvent() override;

private:
	void CreateSubSystems(const QVariantList &Systems);
	void CreateTables(const QVariantList &RightTableType, const QVariantList &Tables);
	void CreateSpecial(const QVariantList &SpecialRights);

private:
	void SubSystemClicked(const QVariant &value); //��������� ������� �� ����� ��� ����������
	void TableClicked(QAction *Action); //��������� ������� �� ����� ��� �������
	void SpecialClicked(const QVariant &value); //��������� ������� �� ����������� �����

private:
	QTabWidget *TabWidget;

private:
	int GroupID;
};
//-----------------------------------------------------------------------------
