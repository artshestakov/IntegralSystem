#include "ISPrintingWord.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISDatabaseHelper.h"
//-----------------------------------------------------------------------------
ISPrintingWord::ISPrintingWord(ISPrintMetaReport *meta_report, int object_id, QObject *parent) : ISPrintingBase(meta_report, object_id, parent)
{
	//WordApplication = nullptr;
	//Documents = nullptr;
	//Document = nullptr;
	//Bookmarks = nullptr;
}
//-----------------------------------------------------------------------------
ISPrintingWord::~ISPrintingWord()
{
	//delete Bookmarks;
	//delete Document;
	//delete Documents;
	//delete WordApplication;
}
//-----------------------------------------------------------------------------
bool ISPrintingWord::Prepare()
{
	/*WordApplication = new QAxObject("Word.Application", this);
	if (!WordApplication)
	{
		SetErrorString(LOCALIZATION("Print.Word.Error.NotInitializeApplication"));
		return false;
	}*/

	/*WordApplication->setProperty("DisplayAlerts", false);
	Documents = WordApplication->querySubObject("Documents");
	if (!Documents)
	{
		SetErrorString(LOCALIZATION("Print.Word.Error.NotInitializeDocuments"));
		return false;
	}*/

	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingWord::PrepareTempate()
{
	SetVisibleDialog(false);
	QString Path;// = ISFileDialog::GetSaveFileName(nullptr, LOCALIZATION("File.Filter.Word"), GetReportLocalName());
	SetVisibleDialog(true);

	if (!Path.length())
	{
		//SetErrorString(LOCALIZATION(""));
		return false;
	}

	//Удаление файла если он уже существует
	QFile FileRemove(Path);
	if (FileRemove.exists())
	{
		if (!FileRemove.remove())
		{
			SetErrorString(FileRemove.errorString());
			return false;
		}
	}

	QFile File(GetMetaReport()->GetFileTemplate());
	if (File.copy(Path))
	{
		File.setFileName(Path);
		bool Setter = File.setPermissions(QFile::WriteOther);
		IS_ASSERT(Setter, File.errorString());
	}
	else
	{
		SetErrorString(LOCALIZATION("Print.Word.Error.NotCopyTemplate").arg(File.errorString()));
		return false;
	}

	//Document = Documents->querySubObject("Add(QVariant)", QVariant(Path));
	Path = Path.replace("/", "\\");
	//Document = Documents->querySubObject("Open(const QString &)", Path);
	//if (!Document)
	{
		SetErrorString(LOCALIZATION("Print.Word.Error.NotOpenTemplate"));
		return false;
	}

	//Bookmarks = Document->querySubObject("Bookmarks()");
	//if (!Bookmarks)
	{
		SetErrorString(LOCALIZATION("Print.Word.Error.NotInitializeBookmarks"));
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingWord::FillTemplate()
{
	QVector<ISPrintMetaReportField*> Fields = GetMetaReport()->GetFields();
	for (int i = 0; i < Fields.count(); ++i)
	{
		ISPrintMetaReportField *MetaReportField = Fields.at(i);

		ISQuery qSelectValue(MetaReportField->GetFieldQuery());
		if (qSelectValue.ExistParameter(":SourceID"))
		{
			bool BindValue = qSelectValue.BindValue(":SourceID", GetObjectID());
			IS_ASSERT(BindValue, "Nit BindValue");
		}

		if (qSelectValue.ExecuteFirst())
		{
			QVariant Value = qSelectValue.ReadColumn(0);
			QVariant CheckedValue = ISDatabaseHelper::CheckValue(Value);
			QString StringValue = CheckedValue.toString();
			
			//QAxObject *WordSelection = WordApplication->querySubObject("Selection");
			//IS_ASSERT(WordSelection, "Not found Selection");

			//QAxObject *Find = WordSelection->querySubObject("Find");
			//IS_ASSERT(Find, "Not found Find");

			//Find->dynamicCall("ClearFormatting()");

			QList<QVariant> Parameters =
			{
				QVariant(MetaReportField->GetReplaceValue()),
				QVariant("0"),
				QVariant("0"),
				QVariant("0"),
				QVariant("0"),
				QVariant("0"),
				QVariant(true),
				QVariant("0"),
				QVariant("0"),
				QVariant(StringValue),
				QVariant("2"),
				QVariant("0"),
				QVariant("0"),
				QVariant("0"),
				QVariant("0"),
			};

			/*Find->dynamicCall("Execute(const QVariant&,const QVariant&,const QVariant&,const QVariant&,"
							  "const QVariant&,const QVariant&,const QVariant&,const QVariant&,"
							  "const QVariant&,const QVariant&,const QVariant&,const QVariant&,"
							  "const QVariant&,const QVariant&,const QVariant&)", Parameters);*/
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingWord::PreviewDocument()
{
	//WordApplication->setProperty("Visible", true);
	//WordApplication->dynamicCall("Activate()");
	return true;
}
//-----------------------------------------------------------------------------
bool ISPrintingWord::Print()
{
	//WordApplication->querySubObject("ActiveDocument")->dynamicCall("Save()");
	//WordApplication->querySubObject("ActiveDocument")->dynamicCall("PrintOut()");
	//WordApplication->querySubObject("ActiveDocument")->dynamicCall("Close()");
	//WordApplication->dynamicCall("Quit()");
	return true;
}
//-----------------------------------------------------------------------------
