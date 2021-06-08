#include "ISTabBarMain.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSettings.h"
#include "ISGui.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISTabBarMain::ISTabBarMain(QWidget *parent)
    : QTabBar(parent),
    MouseClick(false),
    MouseRightClickTabIndex(0)
{
    setObjectName(metaObject()->className());
    setCursor(CURSOR_POINTING_HAND);

    QString SelectionBehavior = SETTING_STRING(CONST_UID_SETTING_TABS_SELECTIONBEHAVIOR);
    if (SelectionBehavior == "SelectLeftTab")
    {
        setSelectionBehaviorOnRemove(QTabBar::SelectionBehavior::SelectLeftTab);
    }
    else if (SelectionBehavior == "SelectRightTab")
    {
        setSelectionBehaviorOnRemove(QTabBar::SelectionBehavior::SelectRightTab);
    }
    else if (SelectionBehavior == "SelectPreviousTab")
    {
        setSelectionBehaviorOnRemove(QTabBar::SelectionBehavior::SelectPreviousTab);
    }

    ContextMenu = new QMenu(this);
    ActionCloseTab = ContextMenu->addAction(BUFFER_ICONS("Tab.Close"), LANG("CloseTab"), this, &ISTabBarMain::CloseTab);
    ActionCloseLeftTabs = ContextMenu->addAction(BUFFER_ICONS("Tab.CloseLeftTab"), LANG("CloseLeftTabs"), this, &ISTabBarMain::CloseLeftTabs);
    ActionCloseRightTabs = ContextMenu->addAction(BUFFER_ICONS("Tab.CloseRightTabs"), LANG("CloseRightTabs"), this, &ISTabBarMain::CloseRightTabs);
    ActionCloseOtherTabs = ContextMenu->addAction(BUFFER_ICONS("Tab.CloseOtherTabs"), LANG("CloseOtherTabs"), this, &ISTabBarMain::CloseOtherTabs);
    ActionCloseAllTabs = ContextMenu->addAction(BUFFER_ICONS("Tab.CloseAllTabs"), LANG("CloseAllTabs"), this, &ISTabBarMain::CloseAllTabs);
    ActionSettings = ContextMenu->addAction(BUFFER_ICONS("Settings.Tabs"), LANG("Settings"), this, &ISTabBarMain::ShowSettingsForm);
    ContextMenu->addSeparator();
    ActionSeparateWindow = ContextMenu->addAction(BUFFER_ICONS("Tab.Separated"), LANG("InSeparateWindow"), this, &ISTabBarMain::SeparateWindowClicked);
}
//-----------------------------------------------------------------------------
ISTabBarMain::~ISTabBarMain()
{

}
//-----------------------------------------------------------------------------
void ISTabBarMain::mousePressEvent(QMouseEvent *e)
{
    QTabBar::mousePressEvent(e);
    MouseRightClickTabIndex = tabAt(e->pos());

    if (e->button() == Qt::LeftButton)
    {
        PressPos = e->localPos();
        MouseClick = true;
    }
    else if (e->button() == Qt::MidButton)
    {
        QTabBar::mousePressEvent(new QMouseEvent(QEvent::Type::MouseButtonPress, e->localPos(), Qt::MouseButton::LeftButton, e->buttons(), e->modifiers()));
        emit MidButtonClicked(currentIndex());
    }
    else if (e->button() == Qt::RightButton) //Вызов контекстного меню
    {
        if (MouseRightClickTabIndex) //Контекстное меню вызывается для вкладок объектов
        {

        }
        else //Контекстное меню вызывается для главной вкладки
        {
            ActionCloseTab->setVisible(false);
            ActionCloseLeftTabs->setVisible(false);
            ActionCloseRightTabs->setVisible(false);
            ActionSeparateWindow->setVisible(false);
        }
        ContextMenu->exec(e->globalPos());
    }
}
//-----------------------------------------------------------------------------
void ISTabBarMain::mouseMoveEvent(QMouseEvent *e)
{
    if (MouseClick)
    {
        if (currentIndex() >= 1)
        {
            if (currentIndex() == 1)
            {
                QPointF ClickPos = e->localPos();
                if (ClickPos.x() < PressPos.x())
                {
                    return;
                }
                else
                {
                    QTabBar::mouseMoveEvent(e);
                }
            }
            else
            {
                QTabBar::mouseMoveEvent(e);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void ISTabBarMain::mouseReleaseEvent(QMouseEvent *e)
{
    QTabBar::mouseReleaseEvent(e);
    MouseClick = false;
    PressPos.setX(0);
    PressPos.setY(0);
}
//-----------------------------------------------------------------------------
void ISTabBarMain::CloseTab()
{
    emit MidButtonClicked(MouseRightClickTabIndex);
    MouseRightClickTabIndex = 0;
}
//-----------------------------------------------------------------------------
void ISTabBarMain::CloseLeftTabs()
{
    for (int i = MouseRightClickTabIndex - 1; i > 0; --i)
    {
        emit MidButtonClicked(i);
    }
    MouseRightClickTabIndex = 0;
}
//-----------------------------------------------------------------------------
void ISTabBarMain::CloseRightTabs()
{
    for (int i = count() - 1; i > MouseRightClickTabIndex; --i)
    {
        emit MidButtonClicked(i);
    }
    MouseRightClickTabIndex = 0;
}
//-----------------------------------------------------------------------------
void ISTabBarMain::CloseOtherTabs()
{
    for (int i = count() - 1; i > 0; --i)
    {
        if (MouseRightClickTabIndex != i)
        {
            emit MidButtonClicked(i);
        }
    }
    MouseRightClickTabIndex = 0;
}
//-----------------------------------------------------------------------------
void ISTabBarMain::CloseAllTabs()
{
    for (int i = count() - 1; i > 0; --i)
    {
        emit MidButtonClicked(i);
    }
    MouseRightClickTabIndex = 0;
}
//-----------------------------------------------------------------------------
void ISTabBarMain::ShowSettingsForm()
{
    ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_TABS);
}
//-----------------------------------------------------------------------------
void ISTabBarMain::SeparateWindowClicked()
{
    emit SeparateWindowSignal(MouseRightClickTabIndex);
    MouseRightClickTabIndex = 0;
}
//-----------------------------------------------------------------------------
