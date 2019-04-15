#pragma once
//-----------------------------------------------------------------------------
#include "isprinting_global.h"
//-----------------------------------------------------------------------------
class ISPRINTING_EXPORT ISHTML : public QObject
{
	Q_OBJECT

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
