#include "ISDiscussionListForm.h"
#include "ISDiscussionObjectForm.h"
//-----------------------------------------------------------------------------
ISDiscussionListForm::ISDiscussionListForm(QWidget *parent) : ISListBaseForm("_Discussion", parent)
{

}
//-----------------------------------------------------------------------------
ISDiscussionListForm::~ISDiscussionListForm()
{

}
//-----------------------------------------------------------------------------
void ISDiscussionListForm::LoadData()
{
	if (GetParentObjectID())
	{
		QString ClassFilter;
		ClassFilter += "dson_tablename = '" + GetParentTableName() + "' \n";
		ClassFilter += "AND dson_objectid = " + QString::number(GetParentObjectID());
		GetQueryModel()->SetClassFilter(ClassFilter);
	}
	ISListBaseForm::LoadData();
}
//-----------------------------------------------------------------------------
void ISDiscussionListForm::Create()
{
	if (ISDiscussionObjectForm(GetParentTableName(), GetParentObjectID(), QString()).Exec())
	{
		Update();
	}
}
//-----------------------------------------------------------------------------
void ISDiscussionListForm::CreateCopy()
{
	QString Message = GetCurrentRecordValue("Message").toString();

	ISDiscussionObjectForm DiscussionObjectForm(GetParentTableName(), GetParentObjectID(), QString());
	DiscussionObjectForm.SetEditMessageText(Message);
	if (DiscussionObjectForm.Exec())
	{
		Update();
	}
}
//-----------------------------------------------------------------------------
void ISDiscussionListForm::Edit()
{
	QString Message = GetCurrentRecordValue("Message").toString();
	int DiscussionID = GetCurrentRecordValue("ID").toInt();

	ISDiscussionObjectForm DiscussionObjectForm(GetParentTableName(), GetParentObjectID(), Message);
	DiscussionObjectForm.SetUID(DiscussionID);
	if (DiscussionObjectForm.Exec())
	{
		Update();
	}
}
//-----------------------------------------------------------------------------
