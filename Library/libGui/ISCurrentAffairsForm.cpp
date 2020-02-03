#include "ISCurrentAffairsForm.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
ISCurrentAffairsForm::ISCurrentAffairsForm(QWidget *parent) : ISListBaseForm("Clients", parent)
{
	GetToolBar()->setVisible(false);
}
//-----------------------------------------------------------------------------
ISCurrentAffairsForm::~ISCurrentAffairsForm()
{

}
//-----------------------------------------------------------------------------
void ISCurrentAffairsForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();
	
	HideField("Address");
	HideField("MainContactPerson");
	HideField("Phone");
	HideField("Comment");
}
//-----------------------------------------------------------------------------
void ISCurrentAffairsForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
	QString FieldName = GetSqlModel()->headerData(ModelIndex.column(), Qt::Horizontal, Qt::UserRole).toString();
	bool Value = GetCurrentRecordValue(FieldName).toBool();
	int CurrentObjectID = GetObjectID();

	ISQuery qUpdate(QString("UPDATE clients SET clts_%1 = :Value WHERE clts_id = :ObjectID").arg(FieldName));
	qUpdate.BindValue(":Value", !Value);
	qUpdate.BindValue(":ObjectID", CurrentObjectID);
	if (qUpdate.Execute())
	{
		SetSelectObjectAfterUpdate(CurrentObjectID);
		Update();
	}
}
//-----------------------------------------------------------------------------
