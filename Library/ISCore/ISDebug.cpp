#include "ISDebug.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
void ShowDebugString(const QString &Severity, const QString &String)
{
	QString output_string = QDateTime::currentDateTime().toString(FORMAT_DATE_TIME_V9);
	output_string += Severity.isEmpty() ? "\t " : "\t[" + Severity + "]\t";
	output_string += String;
	std::cout << output_string.toStdString() << std::endl;
}
//-----------------------------------------------------------------------------
void ShowDebugString(const QString &String)
{
	std::cout << String.toStdString() << std::endl;
}
//-----------------------------------------------------------------------------
