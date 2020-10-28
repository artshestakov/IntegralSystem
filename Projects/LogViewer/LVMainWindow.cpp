#include "LVMainWindow.h"
//-----------------------------------------------------------------------------
LVMainWindow::LVMainWindow(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->addAction("Open file...", this, &LVMainWindow::OpenFile);
	MainLayout->addWidget(ToolBar);

	QTabWidget *TabWidget = new QTabWidget(this);
	MainLayout->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
LVMainWindow::~LVMainWindow()
{

}
//-----------------------------------------------------------------------------
void LVMainWindow::OpenFile()
{

}
//-----------------------------------------------------------------------------
