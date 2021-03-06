#pragma once
#ifndef _ISTASKVIEWFORM_H_INCLUDED
#define _ISTASKVIEWFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISLabels.h"
#include "ISListWidget.h"
#include "ISButtons.h"
#include "ISFieldEdits.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
class ISTaskViewForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void StatusChanged(const ISUuid &NewStatusUID); //������ ��������� ������� ������
	void ConvertedToTask(); //������ �������������� ��������� � ������

public:
	ISTaskViewForm(int task_id, QWidget *parent = 0);
	virtual ~ISTaskViewForm();

protected:
	void keyReleaseEvent(QKeyEvent *KeyEvent);
	void EscapeClicked() override;
	void PasteClicked() override;

private:
	void ReloadStatusButtons(); //������������� ������ � ��������-���������
	void ReopenStatus(); //����������� ������
	void Reopen(); //����������� �����
	void Edit(); //������������� ������
	void CloneTask(); //����������� ������
	void ConvertThisToTask(); //������������� ��� ��������� � ������
	void ConvertListSubTaskToTask(); //������������� ��������� �� ������ � ������
	bool ConvertToTask(int task_id); //������������� ��������� � ������
	void ConvertToSubTask(); //������������� � ���������
	void TaskStatusClicked(); //������� ����� ������� ������
	void SetStatus(const ISUuid &StatusUID); //������� ������ ������
	void ShowStatusHistory(); //�������� ������� ��������� ������� ������
	void UpdateVisibleButtonReOpen();

private:
	void SubTaskLoadList(); //�������� ������ ��������
	void SubTaskCreate(); //�������� ���������
	void SubTaskOpen(QListWidgetItem *ListWidgetItem); //������� ���������
	void SubTaskOpenParent(); //������� ������������ ������

private:
	QToolButton* CreateAddButton(const QString &ToolTip);

private:
	void FileLoadList();
	QWidget* FileCreateWidget(const QPixmap &Pixmap, bool IsImage, const QString &Name, int FileID, const QString &Extension, qint64 Size, const QString &UserFullName, const QDateTime &DateTime);
	void FileAdd();
	void FileShow();
	void FileSave();
	void FileDelete();

private:
	void LinkLoadList();
	void LinkAdd();
	void LinkOpen(QListWidgetItem *ListWidgetItem);
	void LinkDelete();

private:
	void CommentLoadList();
	QWidget* CommentCreateWidget(bool IsParent, int CommentID, unsigned int UserID, const QPixmap &UserPhoto, const QString &UserFIO, const QString &Comment, const QDateTime &DateTime);
	void CommentAdd();
	void CommentEdit();
	void CommentDelete();

private:
	void CheckLoadList();
	QWidget* CreateCheckListWidget(int CheckListID, int Order, const QString &Name, bool Done);
	void CheckAdd();
	void CheckEdit();
	void CheckDelete();
	void CheckClicked(const QVariant &Value);

private:
	QLabel *LabelStatus;
	ISLabelElided *LabelName;
	QHBoxLayout *LayoutHorizontal;
	QVBoxLayout *LayoutLeft;
	QHBoxLayout *LayoutButtonStatus;
	QButtonGroup *ButtonGroupStatus;
	ISPushButton *ButtonReopen;
	ISPushButton *ButtonActions;
	ISTextEdit *TextEdit;
	ISListWidget *ListWidgetSubTask;
	QTabWidget *TabWidget;
	QTreeWidget *TreeWidgetComment;
	std::map<int, QTreeWidgetItem*> MapComment;
	QVBoxLayout *LayoutCheckList;
	ISScrollArea *ScrollCheckList;
	std::vector<QWidget*> VectorCheckList;
	ISListWidget *ListWidgetFiles;
	ISListWidget *ListWidgetLinks;
	QVBoxLayout *LayoutRight;

private:
	int TaskID;
	QString TaskName;
	QString TaskDescription;
	QDate TaskDeadline;
	QString TaskExecutorName;
	QPixmap TaskExecutroPhoto;
	unsigned int TaskExecutorID;
	QString TaskType;
	ISUuid TaskStatusUID;
	QString TaskStatusName;
	QString TaskStatusStyleSheet;
	ISUuid TaskPriorityUID;
	QString TaskPriorityName;
	QPixmap TaskOwnerPhoto;
	QString TaskOwner;
	unsigned int TaskOwnerID;
	bool TaskImportant;
	int TaskParentID;
	QString TaskParentName;
};
//-----------------------------------------------------------------------------
#endif
