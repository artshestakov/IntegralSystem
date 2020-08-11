#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISTabBarMain : public QTabBar
{
	Q_OBJECT

signals:
	void MidButtonClicked(int Index);
	void SeparateWindow(int Index);

public:
	ISTabBarMain(QWidget *parent = 0);
	virtual ~ISTabBarMain();

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
	void CreateContextMenu();
	void SetVisibleContextActions(bool Visible); //Отобразить все кнопки контекстного меню
	
	void CloseAllTabs();
	void ShowSettingsForm();
	void CloseOtherTabs();
	void CloseRightTabs();

private:
	QPointF PressPos;
	bool MouseClick;
	
	QMenu *ContextMenu;
	QAction *ActionCloseTab;
	QAction *ActionCloseRightTabs;
	QAction *ActionCloseOtherTabs;
	QAction *ActionCloseAllTabs;
	QAction *ActionSettings;
	QAction *ActionSeparateWindow;

	int MouseRightClickTabIndex;
};
//-----------------------------------------------------------------------------
