#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISUuid : public QString
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
