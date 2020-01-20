#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISToolBarObject : public QToolBar
{
	Q_OBJECT

signals:
	void ActionClicked(QAction *Action);

public:
	ISToolBarObject(QWidget *parent = 0);
	virtual ~ISToolBarObject();

	QAction* GetCurrentAction(); //�������� ������� ��������
	QAction* CreateAction(const QIcon &Icon, const QString &Text, ISNamespace::ObjectActionType action_type, const QString &TableName = QString(), const QString &ClassName = QString()); //�������� ��������

	void UpdateEnabledActionsList(bool Enabled); //�������� ��������� ��� �������� ��������� �������

protected:
	void ActionTriggered(QAction *ActionTriggered);

private:
	QActionGroup *ActionGroup;
	QAction *CurrentAction;
};
//-----------------------------------------------------------------------------
