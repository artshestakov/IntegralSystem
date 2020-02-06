#include "ISMetaUuidCheckeder.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISMetaUuidCheckeder::ISMetaUuidCheckeder()
{

}
//-----------------------------------------------------------------------------
ISMetaUuidCheckeder::~ISMetaUuidCheckeder()
{

}
//-----------------------------------------------------------------------------
void ISMetaUuidCheckeder::Search(const QFileInfo &FileInfo)
{
    QFile File(FileInfo.filePath());
    if (File.open(QIODevice::ReadOnly))
	{
        Search(File.readAll());
        File.close();
	}
}
//-----------------------------------------------------------------------------
void ISMetaUuidCheckeder::Search(const QString &Content)
{
	QRegExp RegExp(REG_EXP_UID);
	int Pos = 0;

	while ((Pos = RegExp.indexIn(Content, Pos)) != -1)
	{
		QString FindedUID = RegExp.cap(0);
		FindedUID.remove(0, 5);
		ISSystem::RemoveLastSymbolFromString(FindedUID);

		IS_ASSERT(!StringList.contains(FindedUID), QString("UID \"%1\" already exist.").arg(FindedUID));
		StringList.append(FindedUID);
		Pos += RegExp.matchedLength();
	}
}
//-----------------------------------------------------------------------------
