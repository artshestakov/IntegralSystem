#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISTabBarMain : public QTabBar
{
    Q_OBJECT

signals:
    void MidButtonClicked(int Index);
    void SeparateWindowSignal(int Index);

public:
    ISTabBarMain(QWidget *parent = 0);
    virtual ~ISTabBarMain();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void CloseTab(); //������� �������
    void CloseLeftTabs(); //������� ������� �����
    void CloseRightTabs(); //������� ������� ������
    void CloseOtherTabs(); //������� ������ �������
    void CloseAllTabs(); //������� ��� �������
    void ShowSettingsForm(); //���������� ����� �������� ��� �������
    void SeparateWindowClicked();

private:
    QPointF PressPos;
    bool MouseClick;

    QMenu *ContextMenu;
    QAction *ActionCloseTab;
    QAction *ActionCloseLeftTabs;
    QAction *ActionCloseRightTabs;
    QAction *ActionCloseOtherTabs;
    QAction *ActionCloseAllTabs;
    QAction *ActionSettings;
    QAction *ActionSeparateWindow;

    int MouseRightClickTabIndex;
};
//-----------------------------------------------------------------------------
