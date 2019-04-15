#include "StdAfx.h"
#include "ISProtocolListForm.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISProtocolListForm::ISProtocolListForm(QWidget *parent) : ISListBaseForm("_Protocol", parent)
{

}
//-----------------------------------------------------------------------------
ISProtocolListForm::~ISProtocolListForm()
{

}
//-----------------------------------------------------------------------------
void ISProtocolListForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
    Q_UNUSED(ModelIndex);

	QString TableName = GetCurrentRecordValueDB("TableName").toString();
	int ObjectID = GetCurrentRecordValueDB("ObjectID").toInt();
	if (TableName.length() && ObjectID)
	{
		ISGui::CreateObjectForm(ISNamespace::OFT_Edit, TableName, ObjectID)->show();
	}
}
//-----------------------------------------------------------------------------
