#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTabBarMain : public QTabBar
{
	Q_OBJECT

signals:
	void MidButtonClicked(int Index);
	void DuplicateWindow(int Index);
	void SeparateWindow(int Index);
	void FixedTab(int Index);

public:
	ISTabBarMain(QWidget *parent = 0);
	virtual ~ISTabBarMain();

	void AddFixedTab(const QString &TabUID); //��������� �������
	void RemoveFixedTab(const QString &TabUID); //��������� �������
	bool CheckFixedTab(const QString &TabUID); //�������� - ���������� �� �������

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
	void CreateContextMenu();
	void SetVisibleContextActions(bool Visible); //���������� ��� ������ ������������ ����
	
	void CloseAllTabs();
	void ShowSettingsForm();
	void CloseOtherTabs();
	void CloseRightTabs();

private:
	QPointF PressPos;
	bool MouseClick;
	
	QMenu *ContextMenu;
	QAction *ActionFixedTab;
	QAction *ActionCloseTab;
	QAction *ActionCloseRightTabs;
	QAction *ActionCloseOtherTabs;
	QAction *ActionCloseAllTabs;
	QAction *ActionSettings;
	QAction *ActionDuplicate;
	QAction *ActionSeparateWindow;

	int MouseRightClickTabIndex;

	QList<QString> FixedTabs;
};
//-----------------------------------------------------------------------------
