#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISUserGroupWidget : public QWidget
{
	Q_OBJECT

public:
	ISUserGroupWidget(int group_id, const QString &group_name, QWidget *parent = 0);
	virtual ~ISUserGroupWidget();

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
