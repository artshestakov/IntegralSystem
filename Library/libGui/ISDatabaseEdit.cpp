#include "StdAfx.h"
#include "ISDatabaseEdit.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISDatabaseEdit::ISDatabaseEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetDuplicatesEnabled(false);
	SetSizePolicyHorizontal(QSizePolicy::Minimum);

	QFile FileTemp(PATH_TEMP_DIR + "/ConnectDatabases");
	if (FileTemp.open(QIODevice::ReadOnly))
	{
		QStringList StringList = QString(FileTemp.readAll()).split(';');
		FileTemp.close();
		StringList.removeDuplicates();
		StringList.removeOne(QString());
		for (const QString &String : StringList)
		{
			AddItem(String, String);
		}
	}
}
//-----------------------------------------------------------------------------
ISDatabaseEdit::~ISDatabaseEdit()
{

}
//-----------------------------------------------------------------------------
