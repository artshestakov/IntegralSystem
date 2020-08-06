#pragma once
#ifndef _ISTASKVIEWFORM_H_INCLUDED
#define _ISTASKVIEWFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISScrollArea.h"
#include "ISLabels.h"
#include "ISListWidget.h"
#include "ISButtons.h"
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
	void Rename();
	void SetDescription();
	void TaskStatusClicked();

private:
	void FileLoadList();
	void FileAdd();
	void FileSave();
	void FileDelete();

private:
	void LinkLoadList();
	void LinkAdd();
	void LinkOpen();
	void LinkDelete();

private:
	void CommentLoadList();
	QWidget* CommentCreateWidget(int CommentID, const QPixmap &UserPhoto, const QString &UserFullName, const QString &Comment, const QDateTime &DateTime);
	void CommentAdd();
	void CommentEdit();
	void CommentDelete();

private:
	QHBoxLayout *LayoutHorizontal;
	QLabel *LabelDescription;
	QVBoxLayout *LayoutLeft;
	ISPushButton *ButtonProcess;
	ISLabelSelectionText *LabelName;
	QGroupBox *GroupBoxFiles;
	ISListWidget *ListWidgetFiles;
	QGroupBox *GroupBoxLinkTask;
	std::vector<ISLabelLink *> VectorLinks;
	QGroupBox *GroupBoxComments;
	QVBoxLayout *LayoutComments;
	ISScrollArea *ScrollAreaComments;
	std::vector<QWidget *> VectorComments;
	QVBoxLayout *LayoutRight;
	QLabel *LabelStatus;
	QLabel *LabelUpdationDate;

private:
	int TaskID;
	QString TaskName;
	QString TaskDescription;
	QString TaskExecutor;
	QString TaskType;
	ISUuid TaskStatusUID;
	QString TaskStatusName;
	QString TaskPriority;
	QString TaskOwner;
	bool TaskImportant;
	QString TaskCreationDate;
	QString TaskUpdationDate;
};
//-----------------------------------------------------------------------------
#endif
