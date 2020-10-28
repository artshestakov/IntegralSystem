#include "LVMainWindow.h"
#include "LVTabPanel.h"
#include "ISFileDialog.h"
#include "ISLocalization.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
LVMainWindow::LVMainWindow(QWidget *parent) : ISInterfaceForm(parent)
{
	bool ok = ISLocalization::Instance().LoadTraslatorQT();
	ok = ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_INTEGRAL_SYSTEM);

	GetMainLayout()->setContentsMargins(QMargins(0, 0, 0, 0));

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->addAction("Open file...", this, &LVMainWindow::OpenFile);
	GetMainLayout()->addWidget(ToolBar);

	TabWidget = new QTabWidget(this);
	GetMainLayout()->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
LVMainWindow::~LVMainWindow()
{

}
//-----------------------------------------------------------------------------
void LVMainWindow::OpenFile()
{
	QStringList Files = ISFileDialog::GetOpenFileNames(this, "Log file (*.log)");
	for (const QString &FilePath : Files)
	{
		TabWidget->addTab(new LVTabPanel(FilePath, TabWidget), QFileInfo(FilePath).fileName());
	}
}
//-----------------------------------------------------------------------------
