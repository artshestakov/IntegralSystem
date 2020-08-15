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

signals:
	void Renamed(const QString &NewName);
	void DescriptionChanged(const QString &NewDescription);
	void StatusChanged(const ISUuid &NewStatusUID);

public:
	ISTaskViewForm(int task_id, QWidget *parent = 0);
	virtual ~ISTaskViewForm();

protected:
	void keyPressEvent(QKeyEvent *KeyEvent);
	void EscapeClicked() override;

private:
	void Reopen(); //Переоткрыть форму
	void Rename(); //Переименовать задачу
	void SetDescription(); //Изменить описание
	void ConvertToSubTask(); //Преобразовать в подзадачу
	void TaskStatusClicked(); //Событие смены статуса задачи
	void ShowStatusHistory(); //Показать историю изменения статуса задачи

private:
	void SubTaskLoad(); //Загрузка списка подзадач
	void SubTaskCreate(); //Создание подзадачи
	void SubTaskCreated(int task_id); //Событие успешного создания подзадачи
	void SubTaskOpen(QListWidgetItem *ListWidgetItem); //Открыть подзадачу
	void SubTaskOpenParent(); //Открыть родительскую задачу

private:
	QToolButton* CreateAddButton(const QString &ToolTip);

private:
	void FileLoadList();
	QWidget* FileCreateWidget(const QPixmap &Pixmap, const QString &Name, int FileID, const QString &Extension, qint64 Size, const QString &UserFullName, const QString &CreationDate);
	void FileAdd();
	void FileSave();
	void FileDelete();

private:
	void LinkLoadList();
	void LinkAdd();
	void LinkOpen(QListWidgetItem *ListWidgetItem);
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
	QGroupBox *GroupBoxSubTask;
	ISListWidget *ListWidgetSubTask;
	QVBoxLayout *LayoutLeft;
	ISPushButton *ButtonMenu;
	ISPushButton *ButtonProcess;
	ISLabelSelectionText *LabelName;
	QTabWidget *TabWidget;
	QTreeWidget *TreeWidgetComment;
	ISListWidget *ListWidgetFiles;
	ISListWidget *ListWidgetLinks;
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
