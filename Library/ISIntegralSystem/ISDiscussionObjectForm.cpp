#include "StdAfx.h"
#include "ISDiscussionObjectForm.h"
#include "ISDefines.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISCore.h"
#include "ISMessageBox.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
static QString QI_DISCUSSION = PREPARE_QUERY("INSERT INTO _discussion(dson_tablename, dson_objectid, dson_message) "
											  "VALUES(:TableName, :ObjectID, :Message)");
//-----------------------------------------------------------------------------
static QString QU_DISCUSSION = PREPARE_QUERY("UPDATE _discussion SET "
											 "dson_message = :Message "
											 "WHERE dson_id = :DiscussionID");
//-----------------------------------------------------------------------------
ISDiscussionObjectForm::ISDiscussionObjectForm(const QString &table_name, int parent_object_id, const QString &message, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	DiscussionID = 0;
	TableName = table_name;
	ObjectID = parent_object_id;
	Message = message;

	setWindowTitle(LOCALIZATION("Discussion"));
	setWindowIcon(BUFFER_ICONS("Discussion"));

	ForbidResize();

	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	QAction *ActionSave = ISControls::CreateActionSave(ToolBar);
	connect(ActionSave, &QAction::triggered, this, &ISDiscussionObjectForm::Save);
	ToolBar->addAction(ActionSave);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	EditMessage = new ISTextEdit(this);
	EditMessage->SetPlaceholderText(LOCALIZATION("WhatDoYouWantToSay"));
	EditMessage->SetValue(Message);
	FormLayout->addRow(LOCALIZATION("Message") + ":", EditMessage);
}
//-----------------------------------------------------------------------------
ISDiscussionObjectForm::~ISDiscussionObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISDiscussionObjectForm::SetUID(int discussion_id)
{
	DiscussionID = discussion_id;
}
//-----------------------------------------------------------------------------
void ISDiscussionObjectForm::SetEditMessageText(const QString &Message)
{
	EditMessage->SetValue(Message);
}
//-----------------------------------------------------------------------------
void ISDiscussionObjectForm::keyPressEvent(QKeyEvent *e)
{
	if (e->modifiers() == Qt::CTRL && e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
	{
		Save();
	}
}
//-----------------------------------------------------------------------------
void ISDiscussionObjectForm::Save()
{
	if (EditMessage->GetValue().toString().length())
	{
		ISQuery qQuery;

		if (Message.length())
		{
			qQuery = ISQuery(QU_DISCUSSION);
			qQuery.BindValue(":DiscussionID", DiscussionID);
		}
		else
		{
			qQuery = ISQuery(QI_DISCUSSION);
			qQuery.BindValue(":TableName", TableName);
			qQuery.BindValue(":ObjectID", ObjectID);
		}

		qQuery.BindValue(":Message", EditMessage->GetValue());
		bool Execute = qQuery.Execute();
		SetResult(Execute);

		if (Execute)
		{
			close();
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Error.Field.NullValue").arg(LOCALIZATION("Message")));
	}
}
//-----------------------------------------------------------------------------
