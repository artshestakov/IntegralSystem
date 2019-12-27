#include "StdAfx.h"
#include "ISStyleSheet.h"
#include "ISDefines.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
ISStyleSheet::ISStyleSheet()
{
	
}
//-----------------------------------------------------------------------------
ISStyleSheet::~ISStyleSheet()
{

}
//-----------------------------------------------------------------------------
ISStyleSheet& ISStyleSheet::GetInstance()
{
	static ISStyleSheet StyleSheet;
	return StyleSheet;
}
//-----------------------------------------------------------------------------
QString ISStyleSheet::GetStyle(const QString &StyleName, const QString &SourceFile, int FileLine) const
{
	QString StyleSheet;
	if (StyleSheets.contains(StyleName))
	{
		StyleSheet = StyleSheets.value(StyleName);
	}
	else
	{
		IS_ASSERT(false, QString("StyleSheet \"%1\" not found. File: %2; Line: %3").arg(StyleName).arg(SourceFile).arg(FileLine));
	}
	
	return StyleSheet;
}
//-----------------------------------------------------------------------------
void ISStyleSheet::Initialize()
{
	ISCountingTime Time;

	QFileInfoList FileInfoList = QDir(":CSS").entryInfoList(QDir::NoFilter);
	for (int i = 0; i < FileInfoList.count(); i++)
	{
		QFileInfo FileInfo = FileInfoList.at(i);

		QString FileName = FileInfo.completeBaseName();
		QString FilePath = FileInfo.filePath();

		AddStyle(FileName, FilePath);
	}

	ISDebug::ShowDebugString(QString("StyleSheet Initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(StyleSheets.count()));
}
//-----------------------------------------------------------------------------
void ISStyleSheet::AddStyle(const QString &FileName, const QString &FilePath)
{
	IS_ASSERT(!StyleSheets.contains(FileName), "StyleSheet '" + FileName + "' already exist in buffer StyleSheets");

	QFile FileStyle(FilePath);
	IS_ASSERT(FileStyle.exists(), "File " + FileName + " StyleSheet not exist");
	IS_ASSERT(FileStyle.open(QIODevice::ReadOnly), QString("File %1 style sheet not open. Error: %2").arg(FileName).arg(FileStyle.errorString()));
	StyleSheets.insert(FileName, FileStyle.readAll());

	FileStyle.close();
}
//-----------------------------------------------------------------------------
