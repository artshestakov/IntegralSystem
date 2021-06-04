#include "ISTabWidgetMain.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstantsOld.h"
#include "ISObjectFormBase.h"
#include "ISGui.h"
#include "ISDialogsCommon.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISTabWidgetMain::ISTabWidgetMain(QWidget *parent) : QTabWidget(parent)
{
    TabBar = new ISTabBarMain(this);
    TabBar->setStyleSheet(BUFFER_STYLE_SHEET("ISMainWindow.TabBar"));
    connect(TabBar, &ISTabBarMain::MidButtonClicked, this, &ISTabWidgetMain::CloseTabFromIndex);
    connect(TabBar, &ISTabBarMain::SeparateWindowSignal, this, &ISTabWidgetMain::SeparateWindow);
    setTabBar(TabBar);

    setObjectName(metaObject()->className());
    setTabsClosable(true);
    setMovable(true);
    setTabBarAutoHide(true);
    setDocumentMode(false);
    setStyleSheet(BUFFER_STYLE_SHEET("ISMainWindow.TabWidget"));
    setUsesScrollButtons(false);

    ButtonMenu = new QToolButton(this);
    ButtonMenu->setVisible(false);
    ButtonMenu->setCheckable(true);
    ButtonMenu->setAutoRaise(true);
    ButtonMenu->setToolTip(LANG("AllTabs"));
    ButtonMenu->setIcon(BUFFER_ICONS("AllTabs"));
    ButtonMenu->setFixedSize(QSize(32, 32));
    ButtonMenu->setPopupMode(QToolButton::InstantPopup);
    ButtonMenu->setStyleSheet(BUFFER_STYLE_SHEET("QToolButtonMenu"));
    connect(ButtonMenu, &QToolButton::clicked, this, &ISTabWidgetMain::MenuClicked);
}
//-----------------------------------------------------------------------------
ISTabWidgetMain::~ISTabWidgetMain()
{

}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::tabInserted(int Index)
{
    QTabWidget::tabInserted(Index);
    if (Index)
    {
        QString ID = QString::fromStdString(GENERATE_UUID());
        tabBar()->setTabData(Index, ID);

        QToolButton *ButtonClose = new QToolButton(this);
        ButtonClose->setToolTip(LANG("CloseTab"));
        ButtonClose->setIcon(BUFFER_ICONS("CloseTab"));
        ButtonClose->setAutoRaise(true);
        ButtonClose->setProperty("ID", ID);
        ButtonClose->setCursor(CURSOR_POINTING_HAND);
        ButtonClose->setFixedSize(QSize(18, 18));
        connect(ButtonClose, &QToolButton::clicked, this, &ISTabWidgetMain::CloseCliciked);
        tabBar()->setTabButton(Index, QTabBar::RightSide, ButtonClose);
    }
    else
    {
        TabBar->setTabButton(0, QTabBar::RightSide, nullptr);
    }
    ButtonMenu->setVisible(count() > 1);
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::tabRemoved(int index)
{
    QTabWidget::tabRemoved(index);
    ButtonMenu->setVisible(count() > 1);
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::MenuClicked()
{
    ISGui::SetWaitGlobalCursor(true);
    ButtonMenu->setChecked(true);
    QMenu Menu;
    for (int i = 0, c = count(); i < c; ++i)
    {
        QAction *ActionTab = Menu.addAction(tabIcon(i), tabText(i));
        ActionTab->setCheckable(true);
        ActionTab->setData(i);
        if (i == currentIndex())
        {
            ActionTab->setChecked(true);
            ActionTab->setFont(FONT_APPLICATION_BOLD);
        }
    }
    connect(&Menu, &QMenu::triggered, this, &ISTabWidgetMain::TabsMenuTriggered);

    QPoint Point = ButtonMenu->mapToGlobal(QPoint());
    Point.setY(Point.y() + ButtonMenu->height());
    ISGui::SetWaitGlobalCursor(false);
    Menu.exec(Point);
    ButtonMenu->setChecked(false);
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::ReCreateMenu()
{

}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::SeparateWindow(int Index)
{
    ISGui::ShowObjectForm(widget(Index));
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::CloseCliciked()
{
    for (int i = 1, c = count(); i < c; ++i)
    {
        if (tabBar()->tabData(i).toString() == sender()->property("ID").toString())
        {
            CloseTabFromIndex(i);
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::CloseTabFromIndex(int Index)
{
    widget(Index)->close();
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::TabsMenuTriggered(QAction *ActionClicked)
{
    setCurrentIndex(ActionClicked->data().toInt());
}
//-----------------------------------------------------------------------------
