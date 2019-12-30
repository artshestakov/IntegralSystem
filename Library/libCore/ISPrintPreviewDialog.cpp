#include "StdAfx.h"
#include "ISPrintPreviewDialog.h"
#include "ISDefines.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
//#include "ISControls.h"
//-----------------------------------------------------------------------------
ISPrintPreviewDialog::ISPrintPreviewDialog(QWidget *parent, const QString &ReportLocalName, Qt::WindowFlags Flags) : QWidget(parent, Flags | ((Flags & Qt::WindowType_Mask) == 0 ? Qt::Dialog : Qt::WindowType(0)))
{
	Printing = false;
	Show = false;
	EventLoopDialog = nullptr;

	setAttribute(Qt::WA_ShowModal, true);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowIcon(BUFFER_ICONS("Print"));
	setWindowTitle(LOCALIZATION("PreviewPrintWindow"));

	if (!ReportLocalName.isEmpty())
	{
		setWindowTitle(windowTitle() + ": " + ReportLocalName);
	}

	QVBoxLayout *MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(LAYOUT_MARGINS_NULL);
	setLayout(MainLayout);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	MainLayout->addWidget(ToolBar);

	/*QAction *ActionPrint = ISControls::CreateActionPrint(ToolBar);
	ActionPrint->setText(LOCALIZATION("Print"));
	ActionPrint->setToolTip(LOCALIZATION("Print"));
	connect(ActionPrint, &QAction::triggered, this, &ISPrintPreviewDialog::Print);
	ToolBar->addAction(ActionPrint);*/

	QAction *ActionClose = new QAction(ToolBar);
	ActionClose->setText(LOCALIZATION("ClosePreview"));
	ActionClose->setToolTip(LOCALIZATION("ClosePreview"));
	ActionClose->setIcon(BUFFER_ICONS("Close"));
	connect(ActionClose, &QAction::triggered, this, &ISPrintPreviewDialog::close);
	ToolBar->addAction(ActionClose);

	/*PageTextEdit = new ISPagesTextEdit(this);
	PageTextEdit->setPageFormat(QPageSize::A4);
	PageTextEdit->setPageMargins(QMarginsF(15, 15, 15, 15));
	PageTextEdit->setUsePageMode(true);
	PageTextEdit->setPageNumbersAlignment(Qt::AlignBottom | Qt::AlignCenter);
	MainLayout->addWidget(PageTextEdit);*/

	TextDocument = new QTextDocument(this);
	TextDocument->setDefaultFont(FONT_COURIER_12);
	//PageTextEdit->setDocument(TextDocument);

	QStatusBar *StatusBar = new QStatusBar(this);
	StatusBar->setSizeGripEnabled(false);
	MainLayout->addWidget(StatusBar);
}
//-----------------------------------------------------------------------------
ISPrintPreviewDialog::~ISPrintPreviewDialog()
{

}
//-----------------------------------------------------------------------------
void ISPrintPreviewDialog::showEvent(QShowEvent *e)
{
	if (!Show)
	{
		Show = true;
		//PageTextEdit->verticalScrollBar()->setValue(0);
	}

	QWidget::showEvent(e);
}
//-----------------------------------------------------------------------------
void ISPrintPreviewDialog::closeEvent(QCloseEvent *e)
{
	QWidget::closeEvent(e);
	DestroyEventLoop();
}
//-----------------------------------------------------------------------------
void ISPrintPreviewDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		close();
	}

	QWidget::keyPressEvent(e);
}
//-----------------------------------------------------------------------------
void ISPrintPreviewDialog::Exec()
{
	showMaximized();

	QEventLoop EventLoop;
	EventLoopDialog = &EventLoop;
	(void)EventLoop.exec(QEventLoop::DialogExec);
}
//-----------------------------------------------------------------------------
bool ISPrintPreviewDialog::GetPrint() const
{
	return Printing;
}
//-----------------------------------------------------------------------------
void ISPrintPreviewDialog::SetHtml(const QString &html)
{
	TextDocument->setHtml(html);
}
//-----------------------------------------------------------------------------
void ISPrintPreviewDialog::SetReadOnly(bool read_only)
{
	//PageTextEdit->setReadOnly(read_only);
}
//-----------------------------------------------------------------------------
void ISPrintPreviewDialog::DestroyEventLoop()
{
	if (EventLoopDialog)
	{
		EventLoopDialog->exit();
		EventLoopDialog = nullptr;
	}
}
//-----------------------------------------------------------------------------
void ISPrintPreviewDialog::Print()
{
	Printing = true;
	close();
}
//-----------------------------------------------------------------------------
