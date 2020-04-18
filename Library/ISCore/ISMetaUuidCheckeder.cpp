#include "ISMetaUuidCheckeder.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISMetaUuidCheckeder::ISMetaUuidCheckeder()
	: ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISMetaUuidCheckeder::~ISMetaUuidCheckeder()
{

}
//-----------------------------------------------------------------------------
QString ISMetaUuidCheckeder::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISMetaUuidCheckeder::Search(const QFileInfo &FileInfo)
{
    QFile File(FileInfo.filePath());
	bool Result = File.open(QIODevice::ReadOnly);
    if (Result)
	{
		QString Content = File.readAll();
        File.close();
        Result = Search(Content);
	}
	else
	{
		ErrorString = File.errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISMetaUuidCheckeder::Search(const QString &Content)
{
	QRegExp RegExp(REG_EXP_UID);
	int Pos = 0;

	bool Result = true;
	while ((Pos = RegExp.indexIn(Content, Pos)) != -1)
	{
		QString FindedUID = RegExp.cap(0);
		FindedUID.remove(0, 5);
		FindedUID.chop(1);

		Result = !StringList.contains(FindedUID);
		if (!Result)
		{
			ErrorString = QString("UID \"%1\" already exist.").arg(FindedUID);
			break;
		}

		StringList.append(FindedUID);
		Pos += RegExp.matchedLength();
	}
	return Result;
}
//-----------------------------------------------------------------------------
