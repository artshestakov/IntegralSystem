#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISMetaUuidCheckeder
{
public:
	ISMetaUuidCheckeder();
	virtual ~ISMetaUuidCheckeder();

    void Search(const QFileInfo &FileInfo);
	void Search(const QString &Content);

private:
	QStringList StringList;
};
//-----------------------------------------------------------------------------
