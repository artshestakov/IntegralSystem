#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
#include "ISTaskListForm.h"
//-----------------------------------------------------------------------------
class ISTaskParagraph : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskParagraph(QWidget *parent = 0);
	virtual ~ISTaskParagraph();

	void Invoke() override;

private:
	void CreateTempWidget();
	void CreateActionFilter(const QString &Text, const QIcon &Icon, const QString &SqlFilter);
	void FilterClicked();

	void CreateTask();
	void SearchByNumber();
	void SearchByText();

private:
	QVBoxLayout *MainLayout;
	QToolBar *ToolBar;
	QActionGroup *ActionGroup;

	QWidget *TempWidget;
	ISTaskListForm *TaskListForm;
};
//-----------------------------------------------------------------------------
