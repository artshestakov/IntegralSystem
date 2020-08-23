#include "ISPrintingHtml.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISPrintPreviewDialog.h"
#include "ISQuery.h"
#include "ISHtmlQuery.h"
#include "ISAssert.h"
#include "ISMetaViewQuery.h"
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
		bool Contains = Html.contains(MetaReportField->ReplaceValue);
		IS_ASSERT(Contains, QString("Not found replace value \"%1\" in file template \"%2\"").arg(MetaReportField->ReplaceValue).arg(MetaReportField->ParameterName));
		if (!MetaReportField->QueryName.isEmpty())
		{
			ISMetaViewQuery MetaViewQuery(MetaReportField->QueryName);
			QString QueryText = MetaViewQuery.GetQueryText();
			ISQuery qSelect(QueryText);
			qSelect.BindValue(MetaReportField->ParameterName, GetObjectID());
			if (qSelect.Execute())
			{
				if (qSelect.GetCountResultRows())
				{
					IS_ASSERT(qSelect.First(), "Not OnFirst in query: " + QueryText);
					Html.replace(MetaReportField->ReplaceValue, ISHtmlQuery(qSelect, "Services").GetHtmlTableQuery());
				}
				else
				{
					Html.replace(MetaReportField->ReplaceValue, QString());
				}
			}
		}
		else
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
		emit SetVisibleDialog(false);
		QString Path = ISFileDialog::GetSaveFileName(nullptr, LANG("File.Filter.Pdf"), PathPDF);
		emit SetVisibleDialog(true);

		if (Path.length())
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
