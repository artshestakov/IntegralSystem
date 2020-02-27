#include "ISStyleSheet.h"
#include "ISAssert.h"
#include "ISCountingTime.h"
#include "ISLogger.h"
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
	std::map<QString, QString>::const_iterator Iterator = StyleSheets.find(StyleName);
	if(Iterator == StyleSheets.end())
	{
		IS_ASSERT(false, QString("StyleSheet \"%1\" not found. File: %2; Line: %3").arg(StyleName).arg(SourceFile).arg(FileLine));
	}
	return Iterator->second;
}
//-----------------------------------------------------------------------------
void ISStyleSheet::Initialize()
{
	ISCountingTime Time;
	QFileInfoList FileInfoList = QDir(":CSS").entryInfoList(QDir::NoFilter);
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		AddStyle(FileInfo.completeBaseName(), FileInfo.filePath());
	}
	ISLOGGER_DEBUG(QString("StyleSheet Initialized. msec: %1. Items: %2").arg(Time.GetElapsed()).arg(StyleSheets.size()));
}
//-----------------------------------------------------------------------------
void ISStyleSheet::AddStyle(const QString &FileName, const QString &FilePath)
{
	if (StyleSheets.count(FileName))
	{
		IS_ASSERT(false, "StyleSheet '" + FileName + "' already exist in buffer StyleSheets");
	}

	QFile FileStyle(FilePath);
	if (FileStyle.exists())
	{
		if (FileStyle.open(QIODevice::ReadOnly))
		{
			StyleSheets.emplace(FileName, FileStyle.readAll());
			FileStyle.close();
		}
		else
		{
			IS_ASSERT(false, QString("File %1 style sheet not open. Error: %2").arg(FileName).arg(FileStyle.errorString()));
		}
	}
	else
	{
		IS_ASSERT(false, "File " + FileName + " not exist");
	}
}
//-----------------------------------------------------------------------------
