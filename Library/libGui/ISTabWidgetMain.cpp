#include "StdAfx.h"
#include "ISTabWidgetMain.h"
#include "ISSettings.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISObjectFormBase.h"
#include "ISGui.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISTabWidgetMain::ISTabWidgetMain(QWidget *parent) : QTabWidget(parent)
{
	setObjectName(metaObject()->className());

	TabBar = new ISTabBarMain(this);
	connect(TabBar, &ISTabBarMain::FixedTab, this, &ISTabWidgetMain::FixedTab);
	connect(TabBar, &ISTabBarMain::MidButtonClicked, this, &ISTabWidgetMain::CloseTabFromIndex);
	connect(TabBar, &ISTabBarMain::DuplicateWindow, this, &ISTabWidgetMain::DuplicateWindow);
	connect(TabBar, &ISTabBarMain::SeparateWindow, this, &ISTabWidgetMain::SeparateWindow);
	setTabBar(TabBar);
	
	setTabsClosable(true);
	setMovable(true);
	setTabBarAutoHide(true);
	setStyleSheet(STYLE_SHEET("QTabWidget.MainWindow"));
	setUsesScrollButtons(SETTING_BOOL(CONST_UID_SETTING_TABS_VIEWUSESSCROLLBUTTONS));

	QVBoxLayout *LayoutMainTab = new QVBoxLayout();
	LayoutMainTab->setContentsMargins(LAYOUT_MARGINS_NULL);
	LayoutMainTab->setSpacing(0);

	MainTab = new QWidget(this);
	MainTab->setLayout(LayoutMainTab);
	addTab(MainTab, QString());
}
//-----------------------------------------------------------------------------
ISTabWidgetMain::~ISTabWidgetMain()
{

}
//-----------------------------------------------------------------------------
QWidget* ISTabWidgetMain::GetMainTab()
{
	return MainTab;
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::ActivateMainTab()
{
	setCurrentWidget(MainTab);
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::RemoveActionTab(int Index)
{
	for (QAction *Action : ButtonMenu->menu()->actions())
	{
		if (Action->property("ID").toString() == TabBar->tabData(Index).toString())
		{
			ButtonMenu->menu()->removeAction(Action);

			delete Action;
			Action = nullptr;

			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::tabInserted(int Index)
{
	QTabWidget::tabInserted(Index);

	DocumentMode();

	if (Index)
	{
		ISUuid ID = ISSystem::GenerateUuid();
		TabBar->setTabData(Index, ID);

		QToolButton *ButtonClose = new QToolButton(this);
		ButtonClose->setToolTip(LOCALIZATION("CloseTab"));
		ButtonClose->setIcon(BUFFER_ICONS("CloseTab"));
		ButtonClose->setAutoRaise(true);
		ButtonClose->setProperty("ID", ID);
		ButtonClose->setCursor(CURSOR_POINTING_HAND);
		ButtonClose->setFixedSize(SIZE_18_18);
		connect(ButtonClose, &QToolButton::clicked, this, &ISTabWidgetMain::CloseCliciked);
		TabBar->setTabButton(Index, QTabBar::RightSide, ButtonClose);

		QAction *ActionTab = new QAction(ButtonMenu->menu());
		ActionTab->setText(widget(Index)->windowTitle());
		ActionTab->setToolTip(widget(Index)->windowTitle());
		ActionTab->setIcon(widget(Index)->windowIcon());
		ActionTab->setProperty("ID", ID);
		ButtonMenu->menu()->addAction(ActionTab);

		connect(widget(Index), &QWidget::windowTitleChanged, ActionTab, &QAction::setText);
		connect(widget(Index), &QWidget::windowTitleChanged, ActionTab, &QAction::setToolTip);
		connect(widget(Index), &QWidget::windowIconChanged, ActionTab, &QAction::setIcon);
	}
	else
	{
		ButtonMenu = new QToolButton(this);
		ButtonMenu->setAutoRaise(true);
		ButtonMenu->setToolTip(LOCALIZATION("AllTabs"));
		ButtonMenu->setIcon(BUFFER_ICONS("AllTabs"));
		ButtonMenu->setFixedSize(SIZE_18_18);
		ButtonMenu->setPopupMode(QToolButton::InstantPopup);
		ButtonMenu->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));
		TabBar->setTabButton(Index, QTabBar::RightSide, ButtonMenu);

		ButtonMenu->setMenu(new QMenu(ButtonMenu));
		connect(ButtonMenu->menu(), &QMenu::triggered, this, &ISTabWidgetMain::TabsMenuTriggered);
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::tabRemoved(int Index)
{
	QTabWidget::tabRemoved(Index);
	RemoveActionTab(Index);
	DocumentMode();
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::FixedTab(int Index)
{
	QString TabUID = TabBar->tabData(Index).toString();
	if (TabBar->CheckFixedTab(TabUID)) //Если вкладка уже закреплена
	{
		TabBar->RemoveFixedTab(TabUID);
		TabBar->setTabTextColor(Index, COLOR_NULL);
		TabBar->tabButton(Index, QTabBar::RightSide)->setEnabled(true);
	}
	else //Если вкладка не закреплена - закрепить
	{
		TabBar->AddFixedTab(TabUID);
		TabBar->setTabTextColor(Index, Qt::red);
		TabBar->tabButton(Index, QTabBar::RightSide)->setEnabled(false);
		TabBar->moveTab(Index, 1);
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::DuplicateWindow(int Index)
{
	ISObjectFormBase *ObjectFormBase = dynamic_cast<ISObjectFormBase*>(widget(Index));
	if (ObjectFormBase)
	{
		ISObjectFormBase *DuplicateObjectForm = ISGui::CreateObjectForm(ObjectFormBase->GetFormType(), ObjectFormBase->GetMetaTable()->GetName(), ObjectFormBase->GetObjectID());
		emit Duplicate(DuplicateObjectForm);
	}
	else
	{
		ISMessageBox::ShowCritical(this, LOCALIZATION("Message.Warning.DuplicateFormNotObjectFormBase"));
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::SeparateWindow(int Index)
{
	QWidget *Widget = widget(Index);

	RemoveActionTab(Index);
	removeTab(Index);

	Widget->setParent(nullptr);
	ISGui::MoveWidgetToDesktop(Widget, ISNamespace::MWD_Center);
	Widget->show();
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::CloseCliciked()
{
	for (int i = 0; i < count(); ++i) //Обход вкладок
	{
		if (TabBar->tabData(i).toString() == sender()->property("ID").toString())
		{
			CloseTabFromIndex(i);
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::CloseTabFromIndex(int Index)
{
	if (widget(Index)->close())
	{
		RemoveActionTab(Index);
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::TabsMenuTriggered(QAction *ActionClicked)
{
	for (int i = 0; i < ButtonMenu->menu()->actions().count(); ++i)
	{
		QAction *Action = ButtonMenu->menu()->actions().at(i);
		if (Action->property("ID").toString() == ActionClicked->property("ID").toString())
		{
			++i; //Прибавление единицы требуется т.к. обход начинается с НУЛЕВОЙ (главной) вкладки таб-бара
			setCurrentIndex(i);
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::DocumentMode()
{
	if (count() == 1)
	{
		setDocumentMode(true);
	}
	else
	{
		setDocumentMode(false);
	}
}
//-----------------------------------------------------------------------------
