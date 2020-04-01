#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISMetaUuidCheckeder
{
public:
	ISMetaUuidCheckeder();
	virtual ~ISMetaUuidCheckeder();

	QString GetErrorString() const;
    bool Search(const QFileInfo &FileInfo);

private:
	bool Search(const QString &Content);

private:
	QString ErrorString;
	QStringList StringList;
};
//-----------------------------------------------------------------------------
