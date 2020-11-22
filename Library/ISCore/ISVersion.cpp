#include "ISVersion.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISVersion::ISVersion()
	: Major(0),
	Minor(0),
	Revision(0),
	Valid(false)
{

}
//-----------------------------------------------------------------------------
ISVersion::ISVersion(unsigned int major, unsigned int minor, unsigned int revision)
	: Major(major),
	Minor(minor),
	Revision(revision),
	Valid(true)
{

}
//-----------------------------------------------------------------------------
ISVersion::ISVersion(const QString &string) : ISVersion()
{
	QStringList StringList = string.split(SYMBOL_POINT);
	if (StringList.size() != 3) // оличество октетов не равно трем - выходим
	{
		return;
	}

	bool MajorOK = true, MinorOK = true, RevisionOK = true;
	Major = StringList[0].toUInt(&MajorOK);
	Minor = StringList[1].toUInt(&MinorOK);
	Revision = StringList[2].toUInt(&RevisionOK);
	Valid = MajorOK && MinorOK && RevisionOK;
	if (!Valid)
	{
		Clear();
	}
}
//-----------------------------------------------------------------------------
ISVersion::~ISVersion()
{

}
//-----------------------------------------------------------------------------
QString ISVersion::ToString() const
{
	return QString("%1.%2.%3").arg(Major).arg(Minor).arg(Revision);
}
//-----------------------------------------------------------------------------
bool ISVersion::IsValid() const
{
	return Valid;
}
//-----------------------------------------------------------------------------
unsigned int ISVersion::GetMajor() const
{
	return Major;
}
//-----------------------------------------------------------------------------
unsigned int ISVersion::GetMinor() const
{
	return Minor;
}
//-----------------------------------------------------------------------------
unsigned int ISVersion::GetRevision() const
{
	return Revision;
}
//-----------------------------------------------------------------------------
void ISVersion::Clear()
{
	Major = 0;
	Minor = 0;
	Revision = 0;
}
//-----------------------------------------------------------------------------
bool operator==(const ISVersion &v1, const ISVersion &v2)
{
	return v1.Major == v2.Major &&
		v1.Minor == v2.Minor &&
		v1.Revision == v2.Revision;
}
//-----------------------------------------------------------------------------
bool operator!=(const ISVersion &v1, const ISVersion &v2)
{
	return !(v1 == v2);
}
//-----------------------------------------------------------------------------
bool operator>=(const ISVersion &v1, const ISVersion &v2)
{
	return v1.Major >= v2.Major &&
		v1.Minor >= v2.Minor &&
		v1.Revision >= v2.Revision;
}
//-----------------------------------------------------------------------------
bool operator<=(const ISVersion &v1, const ISVersion &v2)
{
	return v1.Major <= v2.Major &&
		v1.Minor <= v2.Minor &&
		v1.Revision <= v2.Revision;
}
//-----------------------------------------------------------------------------
bool operator>(const ISVersion &v1, const ISVersion &v2)
{
	return v1.Major > v2.Major ||
		v1.Minor > v2.Minor ||
		v1.Revision > v2.Revision;
}
//-----------------------------------------------------------------------------
bool operator<(const ISVersion &v1, const ISVersion &v2)
{
	return v1.Major < v2.Major ||
		v1.Minor < v2.Minor ||
		v1.Revision < v2.Revision;
}
//-----------------------------------------------------------------------------
