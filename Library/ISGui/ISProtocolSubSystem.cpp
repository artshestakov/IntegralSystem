#include "ISProtocolSubSystem.h"
#include "ISGui.h"
#include "ISDialogsCommon.h"
#include "ISLocalization.h"
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
	QVariant ObjectID = GetCurrentRecordValue("ObjectID");
	if (ObjectID.isValid() && ObjectID.type() != QVariant::LastCoreType)
	{
		ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit,
			GetCurrentRecordValueDB("TableName").toString(), ObjectID.toInt()));
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.ProtocolNoRecord"));
	}
}
//-----------------------------------------------------------------------------
