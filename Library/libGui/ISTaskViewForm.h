#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceForm.h"
#include "ISTaskCommentsTab.h"
#include "ISTaskFilesTab.h"
#include "ISTaskDuplicatesTab.h"
#include "ISTaskHistoryTab.h"
#include "ISTaskInfoWidget.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskViewForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISTaskViewForm(int task_id, QWidget *parent = 0);
	virtual ~ISTaskViewForm();

protected:
	void SelectTask(); //Запрос к информации по задаче
	void AfterShowEvent() override;
	void EscapeClicked() override;

	void CreateTitle();
	void CreateLeft();
	void CreateRight();

	void Rename();
	void CreateTask();
	void UpdateActions();

	void DoneTask();
	void CloseTask();
	void AddComment();
	void AddFile();
	void CloneTask();
	void Duplicate();
	void Link();
	void ReRead();
	void OpenLinkTask();

private:
	int TaskID; //Идентификатор задачи
	QString TaskName; //Наименование задачи
	bool TaskImportant; //Это важная задача
	QString TaskDescription; //Описание задачи
	int TaskExecutorID; //Исполнитель задачи

	QLabel *LabelName;

	QHBoxLayout *LayoutCenter;
	QAction *ActionDone;
	QAction *ActionClose;
	QAction *ActionProcess;

	ISTaskCommentsTab *TaskCommentsTab;
	ISTaskFilesTab *TaskFilesTab;
	ISTaskDuplicatesTab *TaskDuplicatesTab;
	ISTaskHistoryTab *TaskHistoryTab;

	ISTaskInfoWidget *TaskInfoWidget;
};
//-----------------------------------------------------------------------------
