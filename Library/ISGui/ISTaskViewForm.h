#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISTaskViewForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISTaskViewForm(int task_id, QWidget *parent = 0);
	virtual ~ISTaskViewForm();

protected:
	void EscapeClicked() override;

private:
	void Reopen();

private:
	void LoadComments();
	QWidget* CreateCommentWidget(int CommentID, const QPixmap &UserPhoto, const QString &UserFullName, const QString &Comment, const QDateTime &DateTime);
	void AddComment();
	void EditComment();
	void DeleteComment();

private:
	QHBoxLayout *LayoutHorizontal;
	QVBoxLayout *LayoutLeft;
	QGroupBox *GroupBoxComments;
	ISListWidget *ListWidgetComments;
	QVBoxLayout *LayoutRight;

private:
	int TaskID;
	QString TaskName;
	QString TaskDescription;
	QString TaskExecutor;
	QString TaskType;
	QString TaskStatus;
	QString TaskPriority;
	QString TaskOwner;
	bool TaskImportant;
};
//-----------------------------------------------------------------------------
