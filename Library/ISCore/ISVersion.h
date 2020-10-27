#pragma once
#ifndef _ISVERSION_H_INCLUDED
#define _ISVERSION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISVersion
{
public:
	ISVersion();
	ISVersion(unsigned int major, unsigned int minor, unsigned int revision);
	ISVersion(const QString &string);
	~ISVersion();

	ISCORE_EXPORT friend bool operator==(const ISVersion &v1, const ISVersion &v2);
	ISCORE_EXPORT friend bool operator!=(const ISVersion &v1, const ISVersion &v2);
	ISCORE_EXPORT friend bool operator>=(const ISVersion &v1, const ISVersion &v2);
	ISCORE_EXPORT friend bool operator<=(const ISVersion &v1, const ISVersion &v2);
	ISCORE_EXPORT friend bool operator> (const ISVersion &v1, const ISVersion &v2);
	ISCORE_EXPORT friend bool operator< (const ISVersion &v1, const ISVersion &v2);

	QString ToString() const;
	bool IsValid() const;
	unsigned int GetMajor() const;
	unsigned int GetMinor() const;
	unsigned int GetRevision() const;
	void Clear();

private:
	unsigned int Major; //Мажорный номер
	unsigned int Minor; //Минорный номер
	unsigned int Revision; //Номер ревизии
	bool Valid;
};
//-----------------------------------------------------------------------------
#endif
