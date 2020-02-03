#include "ISVersion.h"
#include "ISDefines.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISVersion::ISVersion()
{
	Major = GetFileData("Major").toInt();
	Minor = GetFileData("Minor").toInt();
	Revision = GetFileData("Revision").toInt();
	Build = GetFileData("Build").toInt();
	Date = QDate::fromString(GetFileData("FromDate"), DATE_FORMAT_V2);
	Time = QTime::fromString(GetFileData("FromTime"), TIME_FORMAT_V1);
	Hash = GetFileData("Hash");
	Platform = GetFileData("Platform");

	VersionString = QString("%1.%2.%3.%4").arg(Major).arg(Minor).arg(Revision).arg(Build);
}
//-----------------------------------------------------------------------------
ISVersion::~ISVersion()
{

}
//-----------------------------------------------------------------------------
ISVersion& ISVersion::GetInstance()
{
	static ISVersion Version;
	return Version;
}
//-----------------------------------------------------------------------------
void ISVersion::Initialize()
{
	
}
//-----------------------------------------------------------------------------
void ISVersion::SetVersion(const QString &version)
{
	QStringList StringList = version.split(SYMBOL_POINT);
	if (StringList.count() == 4)
	{
		VersionString = version;
		Major = StringList.at(0).toInt();
		Minor = StringList.at(1).toInt();
		Revision = StringList.at(2).toInt();
		Build = StringList.at(3).toInt();
	}
}
//-----------------------------------------------------------------------------
bool ISVersion::IsValid() const
{
	if (VersionString.split(SYMBOL_POINT).count() == 4)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
QString ISVersion::GetVersion() const
{
	return VersionString;
}
//-----------------------------------------------------------------------------
int ISVersion::GetMajor() const
{
	return Major;
}
//-----------------------------------------------------------------------------
int ISVersion::GetMinor() const
{
	return Minor;
}
//-----------------------------------------------------------------------------
int ISVersion::GetRevision() const
{
	return Revision;
}
//-----------------------------------------------------------------------------
int ISVersion::GetBuild() const
{
	return Build;
}
//-----------------------------------------------------------------------------
QDate ISVersion::GetDate() const
{
	return Date;
}
//-----------------------------------------------------------------------------
QTime ISVersion::GetTime() const
{
	return Time;
}
//-----------------------------------------------------------------------------
QString ISVersion::GetHash() const
{
	return Hash;
}
//-----------------------------------------------------------------------------
QString ISVersion::GetPlatform() const
{
	return Platform;
}
//-----------------------------------------------------------------------------
QString ISVersion::GetFileData(const QString &FileName) const
{
	QString Result;
	QFile File(":/Version/" + FileName + SYMBOL_POINT + EXTENSION_TXT);
	if (File.open(QIODevice::ReadOnly))
	{
		Result = File.readAll();
		File.close();

		if (Result.contains("\r\n"))
		{
			Result.replace("\r\n", QString());
		}

		if (Result.contains("\n"))
		{
			Result.replace("\n", QString());
		}

		if (Result.right(1) == SYMBOL_SPACE)
		{
			ISSystem::RemoveLastSymbolFromString(Result);
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
