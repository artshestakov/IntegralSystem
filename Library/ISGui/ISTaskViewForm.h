#pragma once
#ifndef _ISTASKVIEWFORM_H_INCLUDED
#define _ISTASKVIEWFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISLabels.h"
#include "ISListWidget.h"
#include "ISButtons.h"
#include "ISTextEdit.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
class ISTaskViewForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void Renamed(const QString &NewName); //Сигнал изменения наименования задачи
	void DescriptionChanged(const QString &NewDescription); //Сигнал изменения описания задачи
	void StatusChanged(const ISUuid &NewStatusUID); //Сигнал изменения статуса задачи
	void ConvertedToTask(); //Сигнал преобразования подзадачи в задачу

public:
	ISTaskViewForm(int task_id, QWidget *parent = 0);
	virtual ~ISTaskViewForm();

protected:
	void keyReleaseEvent(QKeyEvent *KeyEvent);
	void EscapeClicked() override;
	void PasteClicked() override;

private:
	void Reopen(); //Переоткрыть форму
	void Edit(); //Редактировать задачу
	void Rename(); //Переименовать задачу
	void SetDescription(); //Изменить описание
	void CloneTask(); //Клонировать задачу
	void ConvertThisToTask(); //Преобразовать эту подзадачу в задачу
	void ConvertListSubTaskToTask(); //Преобразовать подзадачу из списка в задачу
	bool ConvertToTask(int task_id); //Преобразовать подзадачу в задачу
	void ConvertToSubTask(); //Преобразовать в подзадачу
	void TaskStatusClicked(); //Событие смены статуса задачи
	void ShowStatusHistory(); //Показать историю изменения статуса задачи

private:
	void SubTaskLoadList(); //Загрузка списка подзадач
	void SubTaskCreate(); //Создание подзадачи
	void SubTaskOpen(QListWidgetItem *ListWidgetItem); //Открыть подзадачу
	void SubTaskOpenParent(); //Открыть родительскую задачу

private:
	QToolButton* CreateAddButton(const QString &ToolTip);

private:
	void FileLoadList();
	QWidget* FileCreateWidget(const QPixmap &Pixmap, bool IsImage, const QString &Name, int FileID, const QString &Extension, qint64 Size, const QString &UserFullName, const QDateTime &CreationDate);
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
	QWidget* CommentCreateWidget(int CommentID, const QPixmap &UserPhoto, const QString &UserFullName, const QString &Comment, const QDateTime &DateTime);
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
	QHBoxLayout *LayoutHorizontal;
	ISTextEdit *TextEdit;
	QGroupBox *GroupBoxSubTask;
	QProgressBar *ProgressBarSubTask;
	ISListWidget *ListWidgetSubTask;
	QVBoxLayout *LayoutLeft;
	ISPushButton *ButtonMenu;
	ISPushButton *ButtonProcess;
	ISLabelElided *LabelName;
	QTabWidget *TabWidget;
	QVBoxLayout *LayoutComments;
	ISScrollArea *ScrollComment;
	std::vector<QWidget*> VectorComments;
	QVBoxLayout *LayoutCheckList;
	ISScrollArea *ScrollCheckList;
	std::vector<QWidget*> VectorCheckList;
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
	QString TaskCreationDateToolTip;
	QString TaskUpdationDate;
	QString TaskUpdationDateToolTip;
	int TaskParentID;
	QString TaskParentName;
};
//-----------------------------------------------------------------------------
#endif
