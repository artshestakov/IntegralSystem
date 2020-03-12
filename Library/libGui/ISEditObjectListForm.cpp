#include "ISEditObjectListForm.h"
//-----------------------------------------------------------------------------
ISEditObjectListForm::ISEditObjectListForm(PMetaTable *MetaTable, int ParentObjectID, QWidget *parent) : ISListObjectForm("_Protocol", ParentObjectID, parent)
{
	GetAction(ISNamespace::AT_SystemInfo)->setVisible(false);

	QString ClassFilter;
	ClassFilter += "prtc_protocoltype = '{0361643D-0A62-4F51-84BD-313F53115EFD}' \n";
	//ClassFilter += "AND prtc_tablename = '" + dynamic_cast<ISObjectForm*>(parent)->GetMetaTable()->Name + "' \n";
	ClassFilter += "AND prtc_objectid = " + QString::number(ParentObjectID) + " \n";

	GetQueryModel()->SetClassFilter(ClassFilter);
}
//-----------------------------------------------------------------------------
ISEditObjectListForm::~ISEditObjectListForm()
{

}
//-----------------------------------------------------------------------------
void ISEditObjectListForm::LoadDataAfterEvent()
{
	ISListObjectForm::LoadDataAfterEvent();

	HideField("ObjectID");
	HideField("TableName");
	HideField("TableLocalName");
	HideField("Information");
}
//-----------------------------------------------------------------------------
