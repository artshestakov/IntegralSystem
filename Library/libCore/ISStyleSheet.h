#pragma once
#ifndef _ISSTYLESHEET_H_INCLUDED
#define _ISSTYLESHEET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISStyleSheet
{
public:
	ISStyleSheet(const ISStyleSheet &) = delete;
	ISStyleSheet(ISStyleSheet &&) = delete;
	ISStyleSheet &operator=(const ISStyleSheet &) = delete;
	ISStyleSheet &operator=(ISStyleSheet &&) = delete;
	~ISStyleSheet();

	static ISStyleSheet& GetInstance();
	QString GetStyle(const QString &StyleName, const QString &SourceFile, int FileLine) const;
	void Initialize();

protected:
	void AddStyle(const QString &FileName, const QString &FilePath);

private:
	ISStyleSheet();

	QStringMap StyleSheets;
};
//-----------------------------------------------------------------------------
#define STYLE_SHEET(STYLE_NAME) ISStyleSheet::GetInstance().GetStyle(STYLE_NAME, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
