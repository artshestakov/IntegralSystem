#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGSectionItem
{
public:
	CGSectionItem();
	~CGSectionItem();

	void SetFunction(const QString &function);
	QString GetFunction() const;

	void SetLocalName(const QString &local_name);
	QString GetLocalName() const;

	void SetDescription(const QString &description);
	QString GetDescription() const;

private:
	QString Function;
	QString LocalName;
	QString Description;
};
//-----------------------------------------------------------------------------
