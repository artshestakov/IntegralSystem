#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISTabBarMain.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTabWidgetMain : public QTabWidget
{
	Q_OBJECT

signals:
	void Duplicate(QWidget *DuplicateObjectForm);

public:
	ISTabWidgetMain(QWidget *parent = 0);
	virtual ~ISTabWidgetMain();

	QWidget* GetMainTab(); //�������� ��������� �� ������� �������
	void ActivateMainTab(); //������������ ������� �������
	void RemoveActionTab(int Index);

protected:
	void tabInserted(int Index) override;
	void tabRemoved(int Index) override;
	void FixedTab(int Index);
	void DuplicateWindow(int Index);
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
