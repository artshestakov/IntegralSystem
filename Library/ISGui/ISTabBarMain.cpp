#include "ISTabBarMain.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSettings.h"
#include "ISConstants.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISTabBarMain::ISTabBarMain(QWidget *parent)
	: QTabBar(parent),
	MouseClick(false),
	MouseRightClickTabIndex(0)
{
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
	CreateContextMenu();
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
void ISTabBarMain::wheelEvent(QWheelEvent *e)
{
	if (SETTING_BOOL(CONST_UID_SETTING_TABS_WHEELSCROLLINGTABS))
	{
		QTabBar::wheelEvent(e);
	}
}
//-----------------------------------------------------------------------------
void ISTabBarMain::CreateContextMenu()
{
	ContextMenu = new QMenu(this);
	
	ActionCloseTab = new QAction(BUFFER_ICONS("Tab.Close"), LANG("CloseTab"), ContextMenu);
	connect(ActionCloseTab, &QAction::triggered, this, [=] 
	{ 
		emit MidButtonClicked(MouseRightClickTabIndex);
		MouseRightClickTabIndex = 0;
	});
	ContextMenu->addAction(ActionCloseTab);

	ActionCloseRightTabs = new QAction(BUFFER_ICONS("Tab.CloseRightTabs"), LANG("CloseRightTabs"), ContextMenu);
	connect(ActionCloseRightTabs, &QAction::triggered, this, &ISTabBarMain::CloseRightTabs);
	ContextMenu->addAction(ActionCloseRightTabs);

	ActionCloseOtherTabs = new QAction(BUFFER_ICONS("Tab.CloseOtherTabs"), LANG("CloseOtherTabs"), ContextMenu);
	connect(ActionCloseOtherTabs, &QAction::triggered, this, &ISTabBarMain::CloseOtherTabs);
	ContextMenu->addAction(ActionCloseOtherTabs);

	ActionCloseAllTabs = new QAction(BUFFER_ICONS("Tab.CloseAllTabs"), LANG("CloseAllTabs"), ContextMenu);
	connect(ActionCloseAllTabs, &QAction::triggered, this, &ISTabBarMain::CloseAllTabs);
	ContextMenu->addAction(ActionCloseAllTabs);

	ActionSettings = new QAction(BUFFER_ICONS("Settings.Tabs"), LANG("Settings"), ContextMenu);
	connect(ActionSettings, &QAction::triggered, this, &ISTabBarMain::ShowSettingsForm);
	ContextMenu->addAction(ActionSettings);

	ContextMenu->addSeparator();

	ActionSeparateWindow = new QAction(BUFFER_ICONS("Tab.Separated"), LANG("InSeparateWindow"), ContextMenu);
	connect(ActionSeparateWindow, &QAction::triggered, [=]
	{
		emit SeparateWindow(MouseRightClickTabIndex);
		MouseRightClickTabIndex = 0;
	});
	ContextMenu->addAction(ActionSeparateWindow);
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
void ISTabBarMain::CloseRightTabs()
{
	for (int i = count() - 1; i > MouseRightClickTabIndex; --i)
	{
		emit MidButtonClicked(i);
	}
	MouseRightClickTabIndex = 0;
}
//-----------------------------------------------------------------------------
