#pragma once
#ifndef _ISTASKVIEWFORM_H_INCLUDED
#define _ISTASKVIEWFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISScrollArea.h"
#include "ISLabels.h"
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
	void Rename();
	void SetDescription();

private:
	void LoadFiles();
	void AddFile();
	void SaveFile();
	void DeleteFile();

private:
	void LoadLinks();
	void AddLink();
	void OpenLink();
	void DeleteLink();

private:
	void LoadComments();
	QWidget* CreateCommentWidget(int CommentID, const QPixmap &UserPhoto, const QString &UserFullName, const QString &Comment, const QDateTime &DateTime);
	void AddComment();
	void EditComment();
	void DeleteComment();

private:
	QHBoxLayout *LayoutHorizontal;
	QLabel *LabelDescription;
	QVBoxLayout *LayoutLeft;
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
	QLabel *LabelUpdationDate;

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
	QString TaskCreationDate;
	QString TaskUpdationDate;
};
//-----------------------------------------------------------------------------
#endif
