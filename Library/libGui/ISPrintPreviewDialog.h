#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//#include "ISPagesTextEdit.h"
//-----------------------------------------------------------------------------
class ISPrintPreviewDialog : public QWidget
{
	Q_OBJECT

public:
	ISPrintPreviewDialog(QWidget *parent = 0, const QString &ReportLocalName = QString(), Qt::WindowFlags Flags = 0);
	virtual ~ISPrintPreviewDialog();

	void showEvent(QShowEvent *e);
	void closeEvent(QCloseEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void Exec();

	bool GetPrint() const;
	void SetHtml(const QString &html);
	void SetReadOnly(bool read_only);

protected:
	void DestroyEventLoop();
	void Print();

private:
	bool Printing;
	bool Show;
	QEventLoop *EventLoopDialog;

	//ISPagesTextEdit *PageTextEdit;
	QTextDocument *TextDocument;
};
//-----------------------------------------------------------------------------
