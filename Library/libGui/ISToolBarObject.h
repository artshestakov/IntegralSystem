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

	QAction* GetCurrentAction(); //Получить текущее действие
	QAction* CreateAction(const QIcon &Icon, const QString &Text, ISNamespace::ObjectActionType action_type, const QString &TableName = QString(), const QString &ClassName = QString()); //Добавить действие

	void UpdateEnabledActionsList(bool Enabled); //Обновить видимость для действий эскортных списков

protected:
	void ActionTriggered(QAction *ActionTriggered);

private:
	QActionGroup *ActionGroup;
	QAction *CurrentAction;
};
//-----------------------------------------------------------------------------
