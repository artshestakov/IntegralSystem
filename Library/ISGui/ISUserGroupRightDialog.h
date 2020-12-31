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
	void SubSystemClicked(const QVariant &value); //Обработка нажатия на право для подсистемы
	void TableClicked(QAction *Action); //Обработка нажатия на право для таблицы
	void SpecialClicked(const QVariant &value); //Обработка нажатия на специальное право

private:
	QTabWidget *TabWidget;

private:
	int GroupID;
};
//-----------------------------------------------------------------------------
