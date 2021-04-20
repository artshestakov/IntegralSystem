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

    void CloseTab(); //Закрыть вкладку
    void CloseLeftTabs(); //Закрыть вкладки слева
    void CloseRightTabs(); //Закрыть вкладки справа
    void CloseOtherTabs(); //Закрыть другие вкладки
    void CloseAllTabs(); //Закрыть все вкладки
    void ShowSettingsForm(); //Отобразить форму настроек для вкладок
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
