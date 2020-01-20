#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISTaskForm : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskForm(QWidget *parent = 0);
	virtual ~ISTaskForm();

	void Invoke() override;

protected:
	void CreateTempWidget();
	QAction* CreateActionFilter(const QString &Text, const QIcon &Icon, const QString &ListFormName);
	void FilterClicked();

	void CreateTask();
	void SearchByNumber();

	void CreateObjectForm(QWidget *TaskObjectForm);

private:
	QVBoxLayout *MainLayout;
	QToolBar *ToolBar;
	QActionGroup *ActionGroup;
	QWidget *TempWidget;

	ISTaskBaseListForm *TaskListForm;
	QString CurrentListForm;
};
//-----------------------------------------------------------------------------
