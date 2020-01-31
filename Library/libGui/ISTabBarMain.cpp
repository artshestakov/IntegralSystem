#include "StdAfx.h"
#include "ISTabBarMain.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISSettings.h"
#include "ISConstants.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISTabBarMain::ISTabBarMain(QWidget *parent) : QTabBar(parent)
{
	setObjectName(metaObject()->className());
	setCursor(CURSOR_POINTING_HAND);

	MouseClick = false;
	MouseRightClickTabIndex = 0;

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
void ISTabBarMain::AddFixedTab(const QString &TabUID)
{
	FixedTabs.append(TabUID);
}
//-----------------------------------------------------------------------------
void ISTabBarMain::RemoveFixedTab(const QString &TabUID)
{
	FixedTabs.removeOne(TabUID);
}
//-----------------------------------------------------------------------------
bool ISTabBarMain::CheckFixedTab(const QString &TabUID)
{
	return FixedTabs.contains(TabUID);
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
		SetVisibleContextActions(true);

		if (MouseRightClickTabIndex) //Контекстное меню вызывается для вкладок объектов
		{
			if (FixedTabs.contains(tabData(MouseRightClickTabIndex).toString()))
			{
				ActionFixedTab->setText(LANG("UnfixedTab"));
				ActionFixedTab->setIcon(BUFFER_ICONS("Tab.Unfixed"));
			}
			else
			{
				ActionFixedTab->setText(LANG("FixedTab"));
				ActionFixedTab->setIcon(BUFFER_ICONS("Tab.Fixed"));
			}
		}
		else //Контекстное меню вызывается для главной вкладки
		{
			ActionCloseTab->setVisible(false);
			ActionCloseRightTabs->setVisible(false);
			ActionSeparateWindow->setVisible(false);
			ActionFixedTab->setVisible(false);
		}

		ContextMenu->exec(e->globalPos());
	}
}
//-----------------------------------------------------------------------------
void ISTabBarMain::mouseMoveEvent(QMouseEvent *e)
{
	if (MouseClick)
	{
		if (FixedTabs.contains(tabData(MouseRightClickTabIndex).toString())) //Если вкладка является закрепленной
		{
			return;
		}

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
	
	ActionCloseTab = new QAction(ContextMenu);
	ActionCloseTab->setText(LANG("CloseTab"));
	ActionCloseTab->setIcon(BUFFER_ICONS("Tab.Close"));
	ContextMenu->addAction(ActionCloseTab);
	connect(ActionCloseTab, &QAction::triggered, this, [=] 
	{ 
		emit MidButtonClicked(MouseRightClickTabIndex);
		MouseRightClickTabIndex = 0;
	});

	ActionCloseRightTabs = new QAction(ContextMenu);
	ActionCloseRightTabs->setText(LANG("CloseRightTabs"));
	ActionCloseRightTabs->setIcon(BUFFER_ICONS("Tab.CloseRightTabs"));
	connect(ActionCloseRightTabs, &QAction::triggered, this, &ISTabBarMain::CloseRightTabs);
	ContextMenu->addAction(ActionCloseRightTabs);

	ActionCloseOtherTabs = new QAction(ContextMenu);
	ActionCloseOtherTabs->setText(LANG("CloseOtherTabs"));
	ActionCloseOtherTabs->setIcon(BUFFER_ICONS("Tab.CloseOtherTabs"));
	connect(ActionCloseOtherTabs, &QAction::triggered, this, &ISTabBarMain::CloseOtherTabs);
	ContextMenu->addAction(ActionCloseOtherTabs);

	ActionCloseAllTabs = new QAction(ContextMenu);
	ActionCloseAllTabs->setText(LANG("CloseAllTabs"));
	ActionCloseAllTabs->setIcon(BUFFER_ICONS("Tab.CloseAllTabs"));
	connect(ActionCloseAllTabs, &QAction::triggered, this, &ISTabBarMain::CloseAllTabs);
	ContextMenu->addAction(ActionCloseAllTabs);

	ActionSettings = new QAction(ContextMenu);
	ActionSettings->setText(LANG("Settings"));
	connect(ActionSettings, &QAction::triggered, this, &ISTabBarMain::ShowSettingsForm);
	ContextMenu->addAction(ActionSettings);

	ContextMenu->addSeparator();

	ActionDuplicate = new QAction(ContextMenu);
	ActionDuplicate->setText(LANG("DuplicateTab"));
	ActionDuplicate->setIcon(BUFFER_ICONS("Tab.Duplicate"));
	ContextMenu->addAction(ActionDuplicate);
	connect(ActionDuplicate, &QAction::triggered, [=]
	{
		emit DuplicateWindow(MouseRightClickTabIndex);
		MouseRightClickTabIndex = 0;
	});

	ActionSeparateWindow = new QAction(ContextMenu);
	ActionSeparateWindow->setText(LANG("InSeparateWindow"));
	ActionSeparateWindow->setIcon(BUFFER_ICONS("Tab.Separated"));
	ContextMenu->addAction(ActionSeparateWindow);
	connect(ActionSeparateWindow, &QAction::triggered, [=]
	{
		emit SeparateWindow(MouseRightClickTabIndex);
		MouseRightClickTabIndex = 0;
	});

	ActionFixedTab = new QAction(ContextMenu);
	ActionFixedTab->setText(LANG("FixedTab"));
	ActionFixedTab->setIcon(BUFFER_ICONS("Tab.Fixed"));
	ContextMenu->addAction(ActionFixedTab);
	connect(ActionFixedTab, &QAction::triggered, [=]
	{
		emit FixedTab(MouseRightClickTabIndex);
		MouseRightClickTabIndex = 0;
	});
}
//-----------------------------------------------------------------------------
void ISTabBarMain::SetVisibleContextActions(bool Visible)
{
	for (int i = 0; i < ContextMenu->actions().count(); ++i)
	{
		ContextMenu->actions().at(i)->setVisible(Visible);
	}
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
		if (currentIndex() == i)
		{
			continue;
		}

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
