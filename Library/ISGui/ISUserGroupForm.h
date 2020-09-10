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

	void SubSystemClicked(const QVariant &value); //Обработка нажатия на право для подсистемы
	void TableClicked(QAction *Action); //Обработка нажатия на право для таблицы
	void SpecialClicked(const QVariant &value); //Обработка нажатия на специальное право

private:
	int GroupID;
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
