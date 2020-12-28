#include "ISProtocolSubSystem.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISProtocolSubSystem::ISProtocolSubSystem(QWidget *parent) : ISListBaseForm("_Protocol", parent)
{

}
//-----------------------------------------------------------------------------
ISProtocolSubSystem::~ISProtocolSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISProtocolSubSystem::DoubleClickedTable(const QModelIndex &ModelIndex)
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
