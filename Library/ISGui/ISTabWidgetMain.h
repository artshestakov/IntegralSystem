#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTabBarMain.h"
//-----------------------------------------------------------------------------
class ISTabWidgetMain : public QTabWidget
{
	Q_OBJECT

signals:
	void Duplicate(QWidget *DuplicateObjectForm);

public:
	ISTabWidgetMain(QWidget *parent = 0);
	virtual ~ISTabWidgetMain();

	QWidget* GetMainTab(); //Получить указатель на главную вкладку
	void ActivateMainTab(); //Активировать главную вкладку
	void RemoveActionTab(int Index);

protected:
	void tabInserted(int Index) override;
	void tabRemoved(int Index) override;
	void SeparateWindow(int Index);
	void CloseCliciked();
	void CloseTabFromIndex(int Index);
	void TabsMenuTriggered(QAction *ActionClicked);
	void DocumentMode();

private:
	QWidget *MainTab;
	ISTabBarMain *TabBar;
	QToolButton *ButtonMenu;
};
//-----------------------------------------------------------------------------
