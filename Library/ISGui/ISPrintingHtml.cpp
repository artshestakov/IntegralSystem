#include "ISPrintingHtml.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISPrintPreviewDialog.h"
#include "ISQuery.h"
#include "ISHtmlQuery.h"
#include "ISAssert.h"
#include "ISDatabaseHelper.h"
#include "ISGui.h"
#include "ISFileDialog.h"
//-----------------------------------------------------------------------------
ISPrintingHtml::ISPrintingHtml(ISPrintMetaReport *meta_report, int object_id, QObject *parent)
	: ISPrintingBase(meta_report, object_id, parent),
	PDF(false),
	EditPreview(false)
{
	
}
//-----------------------------------------------------------------------------
ISPrintingHtml::~ISPrintingHtml()
{

}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::Prepare()
{
	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::PrepareTempate()
{
	QFile File(GetMetaReport()->FileTemplate);
	bool Result = File.open(QIODevice::ReadOnly | QIODevice::Text);
	if (Result)
	{
		Html = File.readAll();
		File.close();
	}
	else
	{
		SetErrorString(File.errorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::FillTemplate()
{
	for (ISPrintMetaReportField *MetaReportField : GetMetaReport()->Fields)
	{
		ISQuery qSelectValue(MetaReportField->FieldQuery);
		if (qSelectValue.ExistParameter(":SourceID"))
		{
			IS_ASSERT(qSelectValue.BindValue(":SourceID", GetObjectID()), "Not BindValue");
		}

		if (qSelectValue.ExecuteFirst())
		{
			QVariant Value = qSelectValue.ReadColumn(0);
			ISDatabaseHelper::CheckValue(Value);
			Html.replace(MetaReportField->ReplaceValue, Value.toString());
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::PreviewDocument()
{
	ISGui::SetWaitGlobalCursor(false);
	emit SetVisibleDialog(false);

	ISPrintPreviewDialog PrintPreviewDialog(nullptr, GetReportLocalName());
	PrintPreviewDialog.SetHtml(Html);
	PrintPreviewDialog.SetReadOnly(EditPreview);
	PrintPreviewDialog.Exec();
	bool Result = PrintPreviewDialog.GetPrint();

	ISGui::SetWaitGlobalCursor(true);
	emit SetVisibleDialog(true);

	return Result;
}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::Print()
{
	QPrinter Printer(QPrinter::ScreenResolution);
	Printer.setPageSize(QPrinter::A4);
	Printer.setOrientation(QPrinter::Portrait);
	
	QTextDocument TextDocument;
	TextDocument.setHtml(Html);

	if (PDF)
	{
		emit SetVisibleDialog(false); //Скрываем диалог на время выбора директории сохранения
		QString Path = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Pdf"), PathPDF);
		emit SetVisibleDialog(true); //Отображаем диалог обратно
		if (!Path.isEmpty())
		{
			emit Message(LANG("PrintProcess.SavedToPDF"));
			Printer.setOutputFormat(QPrinter::PdfFormat);
			Printer.setOutputFileName(Path);
			TextDocument.print(&Printer);
		}
	}
	else
	{
		QPrintDialog PrintDialog(&Printer);
		ISGui::MoveWidgetToDesktop(&PrintDialog, ISNamespace::MWD_Center);
		PrintDialog.setWindowTitle(LANG("SelectPrinter"));
		PrintDialog.setWindowIcon(BUFFER_ICONS("Print"));
		if (PrintDialog.exec() == QPrintDialog::Accepted)
		{
			TextDocument.print(&Printer);
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::GetPDF() const
{
	return PDF;
}
//-----------------------------------------------------------------------------
void ISPrintingHtml::SetPDF(bool pdf)
{
	PDF = pdf;
}
//-----------------------------------------------------------------------------
QString ISPrintingHtml::GetPathPDF() const
{
	return PathPDF;
}
//-----------------------------------------------------------------------------
void ISPrintingHtml::SetPathPDF(const QString &path_pdf)
{
	PathPDF = path_pdf;
}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::GetEditPreview() const
{
	return EditPreview;
}
//-----------------------------------------------------------------------------
void ISPrintingHtml::SetEditPreview(bool edit_preview)
{
	EditPreview = edit_preview;
}
//-----------------------------------------------------------------------------
