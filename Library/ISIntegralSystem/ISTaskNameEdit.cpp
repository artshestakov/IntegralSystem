#include "StdAfx.h"
#include "ISTaskNameEdit.h"
#include "EXDefines.h"
//-----------------------------------------------------------------------------
ISTaskNameEdit::ISTaskNameEdit(QWidget *parent) : ISLineEdit(parent)
{
	Completer = nullptr;

	QFile File(APPLICATION_TEMP_PATH + "/TaskNameLog");
	if (File.exists())
	{
		if (File.open(QIODevice::ReadOnly))
		{
			QString String = File.readAll();
			StringList = String.split("\n");
			StringList.removeOne(QString());
			File.close();

			CreateCompleter(StringList);
		}
	}
}
//-----------------------------------------------------------------------------
ISTaskNameEdit::~ISTaskNameEdit()
{

}
//-----------------------------------------------------------------------------
void ISTaskNameEdit::Invoke()
{
	QString TaskName = GetValue().toString();
	if (!StringList.contains(TaskName))
	{
		StringList.append(TaskName);
	}

	StringList.removeDuplicates();
	StringList.sort();

	QFile File(APPLICATION_TEMP_PATH + "/TaskNameLog");
	if (File.open(QIODevice::Truncate | QIODevice::Append))
	{
		QTextStream TextStream(&File);
		for (const QString &String : StringList)
		{
			TextStream << String << endl;
		}
		File.close();
	}

	CreateCompleter(StringList);
}
//-----------------------------------------------------------------------------
void ISTaskNameEdit::CreateCompleter(const QStringList &StringList)
{
	if (Completer)
	{
		delete Completer;
		Completer = nullptr;
	}

	Completer = new QCompleter(StringList, this);
	Completer->setMaxVisibleItems(20);
	Completer->setFilterMode(Qt::MatchContains);
	Completer->setCaseSensitivity(Qt::CaseInsensitive);
	Completer->setCompletionMode(QCompleter::PopupCompletion);
	SetCompleter(Completer);
}
//-----------------------------------------------------------------------------
