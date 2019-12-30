#include "StdAfx.h"
#include "ISHTML.h"
#include "ISAssert.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISHTML::ISHTML(QFile &FileTemplate) : QObject()
{
	IS_ASSERT(FileTemplate.isOpen(), "File '" + FileTemplate.fileName() + "' not opened");
	HTML = QString::fromLocal8Bit(FileTemplate.readAll());
	FilePath = FileTemplate.fileName();
}
//-----------------------------------------------------------------------------
ISHTML::~ISHTML()
{

}
//-----------------------------------------------------------------------------
QString ISHTML::GetHTML() const
{
	return HTML;
}
//-----------------------------------------------------------------------------
void ISHTML::Replace(const QString &ReplacedText, const QString &Text)
{
	bool Contains = HTML.contains(ReplacedText);
	IS_ASSERT(Contains, "String '" + ReplacedText + "' not found in file '" + FilePath + "'");
	HTML.replace(ReplacedText, Text);
}
//-----------------------------------------------------------------------------
void ISHTML::SetTableHtmlName(const QString &Name)
{
	bool Contains = HTML.contains("%TableName%");
	IS_ASSERT(Contains, "");
	HTML.replace("%TableName%", Name);
}
//-----------------------------------------------------------------------------
