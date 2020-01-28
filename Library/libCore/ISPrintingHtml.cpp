#include "ISPrintingHtml.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
//#include "ISPrintPreviewDialog.h"
#include "ISQuery.h"
#include "ISHtmlQuery.h"
#include "ISAssert.h"
#include "ISMetaViewQuery.h"
#include "ISDatabaseHelper.h"
//-----------------------------------------------------------------------------
ISPrintingHtml::ISPrintingHtml(ISPrintMetaReport *meta_report, int object_id, QObject *parent) : ISPrintingBase(meta_report, object_id, parent)
{
	PDF = false;
	EditPreview = false;
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
	QFile File(GetMetaReport()->GetFileTemplate());
	if (File.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		Html = File.readAll();
		File.close();
	}
	else
	{
		SetErrorString(File.errorString());
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::FillTemplate()
{
	QVector<ISPrintMetaReportField*> Fields = GetMetaReport()->GetFields();
	for (int i = 0; i < Fields.count(); ++i)
	{
		ISPrintMetaReportField *MetaReportField = Fields.at(i);

		bool Contains = Html.contains(MetaReportField->GetReplaceValue());
		IS_ASSERT(Contains, QString("Not found replace value \"%1\" in file template \"%2\"").arg(MetaReportField->GetReplaceValue()).arg(""/*File.fileName()*/));

		if (MetaReportField->GetQueryName().length())
		{
			ISMetaViewQuery MetaViewQuery(MetaReportField->GetQueryName());
			QString QueryText = MetaViewQuery.GetQueryText();
			ISQuery qSelect(QueryText);
			qSelect.BindValue(MetaReportField->GetParameterName(), GetObjectID());
			if (qSelect.Execute())
			{
				if (qSelect.GetCountResultRows())
				{
					bool First = qSelect.First();
					IS_ASSERT(First, "Not OnFirst in query: " + QueryText);

					ISHtmlQuery HtmlQuery(qSelect, "Services");
					QString Html = HtmlQuery.GetHtmlTableQuery();
					Html.replace(MetaReportField->GetReplaceValue(), Html);
				}
				else
				{
					Html.replace(MetaReportField->GetReplaceValue(), QString());
				}
			}
		}
		else
		{
			ISQuery qSelectValue(MetaReportField->GetFieldQuery());

			if (qSelectValue.ExistParameter(":SourceID"))
			{
				bool BindValue = qSelectValue.BindValue(":SourceID", GetObjectID());
				IS_ASSERT(BindValue, "Not BindValue");
			}

			if (qSelectValue.ExecuteFirst())
			{
				QVariant Value = qSelectValue.ReadColumn(0);
				QVariant CheckedValue = ISDatabaseHelper::CheckValue(Value);
				QString StringValue = CheckedValue.toString();
				Html.replace(MetaReportField->GetReplaceValue(), StringValue);
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingHtml::PreviewDocument()
{
	//???
	/*ISGui::SetWaitGlobalCursor(false);
	emit SetVisibleDialog(false);

	ISPrintPreviewDialog PrintPreviewDialog(nullptr, GetReportLocalName());
	PrintPreviewDialog.SetHtml(Html);
	PrintPreviewDialog.SetReadOnly(EditPreview);
	PrintPreviewDialog.Exec();
	bool Result = PrintPreviewDialog.GetPrint();

	ISGui::SetWaitGlobalCursor(true);
	emit SetVisibleDialog(true);

	return Result;*/
	return true;
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
		QString Path = QFileDialog::getSaveFileName(nullptr, LOCALIZATION("PrintSavePDF"), PathPDF, LOCALIZATION("File.Filter.Pdf"));
		emit SetVisibleDialog(true);

		if (Path.length())
		{
			emit Message(LOCALIZATION("PrintProcess.SavedToPDF"));

			Printer.setOutputFormat(QPrinter::PdfFormat);
			Printer.setOutputFileName(Path);

			TextDocument.print(&Printer);
		}
	}
	else
	{
		//???
		/*QPrintDialog PrintDialog(&Printer);
		ISGui::MoveWidgetToDesktop(&PrintDialog, ISNamespace::MWD_Center);
		PrintDialog.setWindowTitle(LOCALIZATION("SelectPrinter"));
		PrintDialog.setWindowIcon(BUFFER_ICONS("Print"));
		if (PrintDialog.exec() == QPrintDialog::Accepted)
		{
			TextDocument.print(&Printer);
		}*/
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
