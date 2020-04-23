#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISHTML
{
public:
	ISHTML(QFile &FileTemplate);
	virtual ~ISHTML();

	QString GetHTML() const;
	void Replace(const QString &ReplacedText, const QString &Text);
	void SetTableHtmlName(const QString &Name);

private:
	QString HTML;
	QString FilePath;
};
//-----------------------------------------------------------------------------
