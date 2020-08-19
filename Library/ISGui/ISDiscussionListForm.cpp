#include "ISDiscussionListForm.h"
#include "ISQuery.h"
#include "ISInputDialog.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QI_DISCUSSION = PREPARE_QUERY("INSERT INTO _discussion(dson_tablename, dson_objectid, dson_message) "
											  "VALUES(:TableName, :ObjectID, :Message) "
											  "RETURNING dson_id");
//-----------------------------------------------------------------------------
static QString QU_DISCUSSION = PREPARE_QUERY("UPDATE _discussion SET "
											 "dson_message = :Message "
											 "WHERE dson_id = :DiscussionID");
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
	bool Ok = true;
	QString Message = ISInputDialog::GetText(LANG("Discussion"), LANG("WhatDoYouWantToSay"), QVariant(), Ok);
	if (Ok && !Message.isEmpty())
	{
		ISQuery qInsert(QI_DISCUSSION);
		qInsert.BindValue(":TableName", GetParentTableName());
		qInsert.BindValue(":ObjectID", GetParentObjectID());
		qInsert.BindValue(":Message", Message);
		if (qInsert.ExecuteFirst())
		{
			SetSelectObjectAfterUpdate(qInsert.ReadColumn("dson_id").toInt());
			Update();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qInsert.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISDiscussionListForm::CreateCopy()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CopyDiscussion")))
	{
		ISQuery qInsert(QI_DISCUSSION);
		qInsert.BindValue(":TableName", GetParentTableName());
		qInsert.BindValue(":ObjectID", GetParentObjectID());
		qInsert.BindValue(":Message", GetCurrentRecordValue("Message"));
		if (qInsert.ExecuteFirst())
		{
			SetSelectObjectAfterUpdate(qInsert.ReadColumn("dson_id").toInt());
			Update();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qInsert.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISDiscussionListForm::Edit()
{
	QString Message = GetCurrentRecordValue("Message").toString();
	int DiscussionID = GetCurrentRecordValue("ID").toInt();

	bool Ok = true;
	QString NewMessage = ISInputDialog::GetText(LANG("Discussion"), LANG("WhatDoYouWantToSay"), Message, Ok);
	if (Ok && !NewMessage.isEmpty() && NewMessage != Message)
	{
		ISQuery qUpdate(QU_DISCUSSION);
		qUpdate.BindValue(":Message", NewMessage);
		qUpdate.BindValue(":DiscussionID", DiscussionID);
		if (qUpdate.Execute())
		{
			SetSelectObjectAfterUpdate(DiscussionID);
			Update();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qUpdate.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
