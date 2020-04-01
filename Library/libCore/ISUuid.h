#pragma once
#ifndef _ISUUID_H_INCLUDED
#define _ISUUID_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISUuid : public QString
{
public:
	ISUuid(const QString &UID);
	ISUuid(const QVariant &UID);
	ISUuid(const QUuid &UID);
	ISUuid(const char *UID);
	ISUuid();
	virtual ~ISUuid();

	bool IsValid() const;
};
//-----------------------------------------------------------------------------
#endif
