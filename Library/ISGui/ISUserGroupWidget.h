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

	void SubSystemClicked(const QVariant &value); //��������� ������� �� ����� ��� ����������
	void TableClicked(QAction *Action); //��������� ������� �� ����� ��� �������
	void SpecialClicked(const QVariant &value); //��������� ������� �� ����������� �����

private:
	int GroupID;
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
