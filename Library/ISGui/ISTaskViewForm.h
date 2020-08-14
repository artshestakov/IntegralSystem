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
	void ShowStatusHistory();
	void CreateSubTask();

private:
	QToolButton* CreateAddButton(const QString &ToolTip);

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
	QTabWidget *TabWidget;
	QTreeWidget *TreeWidgetComment;
	int TreeWidgetCommentIndex;
	QGroupBox *GroupBoxFiles;
	ISListWidget *ListWidgetFiles;
	QGroupBox *GroupBoxLinkTask;
	std::vector<ISLabelPixmapText*> VectorLinks;
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
	ISUuid TaskPriorityUID;
	QString TaskPriorityName;
	QString TaskOwner;
	bool TaskImportant;
	QString TaskCreationDate;
	QString TaskUpdationDate;
	int TaskParentID;
	QString TaskParentName;
};
//-----------------------------------------------------------------------------
#endif
