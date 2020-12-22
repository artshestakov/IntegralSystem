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
	QVariant TableName = GetCurrentRecordValueDB("TableName");
	QVariant ObjectID = GetCurrentRecordValueDB("ObjectID");
	if (TableName.isValid() && ObjectID.isValid())
	{
		ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, TableName.toString(), ObjectID.toInt()));
	}
}
//-----------------------------------------------------------------------------
