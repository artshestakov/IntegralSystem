#include "ISTabWidgetMain.h"
#include "ISDefinesGui.h"
#include "ISSettings.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISObjectFormBase.h"
#include "ISGui.h"
#include "ISMessageBox.h"
#include "ISTabBarMain.h"
//-----------------------------------------------------------------------------
ISTabWidgetMain::ISTabWidgetMain(QWidget *parent) : QTabWidget(parent)
{
	ISTabBarMain *TabBar = new ISTabBarMain(this);
	TabBar->setStyleSheet(STYLE_SHEET("ISMainWindow.TabBar"));
	connect(TabBar, &ISTabBarMain::MidButtonClicked, this, &ISTabWidgetMain::CloseTabFromIndex);
	connect(TabBar, &ISTabBarMain::SeparateWindowSignal, this, &ISTabWidgetMain::SeparateWindow);
	setTabBar(TabBar);

	setObjectName(metaObject()->className());
	setTabsClosable(true);
	setMovable(true);
	setTabBarAutoHide(true);
	setStyleSheet(STYLE_SHEET("ISMainWindow.TabWidget"));
	setUsesScrollButtons(SETTING_BOOL(CONST_UID_SETTING_TABS_VIEWUSESSCROLLBUTTONS));

	QVBoxLayout *LayoutMainTab = new QVBoxLayout();
	LayoutMainTab->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutMainTab->setSpacing(0);

	MainTab = new QWidget(this);
	MainTab->setLayout(LayoutMainTab);
	int IndexMainTab = addTab(MainTab, QString());

	ButtonMenu = new QToolButton(this);
	ButtonMenu->setAutoRaise(true);
	ButtonMenu->setToolTip(LANG("AllTabs"));
	ButtonMenu->setIcon(BUFFER_ICONS("AllTabs"));
	ButtonMenu->setFixedSize(ISDefines::Gui::SIZE_18_18);
	ButtonMenu->setPopupMode(QToolButton::InstantPopup);
	ButtonMenu->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));
	//ButtonMenu->setMenu(new QMenu(ButtonMenu));
	connect(ButtonMenu, &QToolButton::clicked, this, &ISTabWidgetMain::MenuClicked);
	tabBar()->setTabButton(IndexMainTab, QTabBar::RightSide, ButtonMenu);
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
void ISTabWidgetMain::tabInserted(int Index)
{
	QTabWidget::tabInserted(Index);
	if (Index)
	{
		ISUuid ID = ISSystem::GenerateUuid();
		tabBar()->setTabData(Index, ID);

		QToolButton *ButtonClose = new QToolButton(this);
		ButtonClose->setToolTip(LANG("CloseTab"));
		ButtonClose->setIcon(BUFFER_ICONS("CloseTab"));
		ButtonClose->setAutoRaise(true);
		ButtonClose->setProperty("ID", ID);
		ButtonClose->setCursor(CURSOR_POINTING_HAND);
		ButtonClose->setFixedSize(ISDefines::Gui::SIZE_18_18);
		connect(ButtonClose, &QToolButton::clicked, this, &ISTabWidgetMain::CloseCliciked);
		tabBar()->setTabButton(Index, QTabBar::RightSide, ButtonClose);
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetMain::MenuClicked()
{
	ISGui::SetWaitGlobalCursor(true);
	QMenu Menu;
	for (int i = 1, c = count(); i < c; ++i)
	{
		QWidget *TabWidget = widget(i);
		QAction *ActionTab = Menu.addAction(TabWidget->windowIcon(), TabWidget->windowTitle());
		ActionTab->setCheckable(true);
		ActionTab->setData(i);
		if (i == currentIndex())
		{
			ActionTab->setChecked(true);
			ActionTab->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		}
	}
	connect(&Menu, &QMenu::triggered, this, &ISTabWidgetMain::TabsMenuTriggered);

	QPoint Point = ButtonMenu->mapToGlobal(QPoint());
	Point.setY(Point.y() + ButtonMenu->height());
	ISGui::SetWaitGlobalCursor(false);
	Menu.exec(Point);
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
