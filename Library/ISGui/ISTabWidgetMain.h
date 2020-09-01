#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISTabWidgetMain : public QTabWidget
{
	Q_OBJECT

signals:
	void Duplicate(QWidget *DuplicateObjectForm);

public:
	ISTabWidgetMain(QWidget *parent = 0);
	virtual ~ISTabWidgetMain();

	QWidget* GetMainTab(); //ѕолучить указатель на главную вкладку

protected:
	void tabInserted(int Index) override;

private:
	void MenuClicked();
	void ReCreateMenu();
	void SeparateWindow(int Index);
	void CloseCliciked();
	void CloseTabFromIndex(int Index);
	void TabsMenuTriggered(QAction *ActionClicked);

private:
	QWidget *MainTab;
	QToolButton *ButtonMenu;
};
//-----------------------------------------------------------------------------
