#include "ISDiscussionListForm.h"
#include "ISTcpQuery.h"
#include "ISInputDialog.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
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
		GetTcpQuery()->AddFilter("TableName", GetParentTableName());
		GetTcpQuery()->AddFilter("ObjectID", GetParentObjectID());
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
		ISTcpQuery qDiscussionAdd(API_DISCUSSION_ADD);
		qDiscussionAdd.BindValue("TableName", GetParentTableName());
		qDiscussionAdd.BindValue("ObjectID", GetParentObjectID());
		qDiscussionAdd.BindValue("Message", Message);
		if (qDiscussionAdd.Execute())
		{
			SetSelectObjectAfterUpdate(qDiscussionAdd.GetAnswer()["ID"].toUInt());
			Update();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qDiscussionAdd.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISDiscussionListForm::CreateCopy()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CopyDiscussion")))
	{
		ISTcpQuery qDiscussionCopy(API_DISCUSSION_COPY);
		qDiscussionCopy.BindValue("ID", GetObjectID());
		if (qDiscussionCopy.Execute())
		{
			SetSelectObjectAfterUpdate(qDiscussionCopy.GetAnswer()["ID"].toUInt());
			Update();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qDiscussionCopy.GetErrorString());
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
		ISTcpQuery qDiscussionEdit(API_DISCUSSION_EDIT);
		qDiscussionEdit.BindValue("ID", GetObjectID());
		qDiscussionEdit.BindValue("Message", NewMessage);
		if (qDiscussionEdit.Execute())
		{
			SetSelectObjectAfterUpdate(DiscussionID);
			Update();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qDiscussionEdit.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
