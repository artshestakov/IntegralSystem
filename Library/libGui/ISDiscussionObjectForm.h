#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class ISDiscussionObjectForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISDiscussionObjectForm(const QString &table_name, int parent_object_id, const QString &message, QWidget *parent = 0);
	virtual ~ISDiscussionObjectForm();

	void SetUID(int discussion_id);
	void SetEditMessageText(const QString &Message);

protected:
	void keyPressEvent(QKeyEvent *e);
	void Save();

private:
	int DiscussionID;
	QString TableName;
	int ObjectID;
	QString Message;

	ISTextEdit *EditMessage;
};
//-----------------------------------------------------------------------------
