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

protected:
	void tabInserted(int Index) override;
	void tabRemoved(int index) override;

private:
	void MenuClicked();
	void ReCreateMenu();
	void SeparateWindow(int Index);
	void CloseCliciked();
	void CloseTabFromIndex(int Index);
	void TabsMenuTriggered(QAction *ActionClicked);

private:
	ISTabBarMain *TabBar;
	QToolButton *ButtonMenu;
};
//-----------------------------------------------------------------------------
