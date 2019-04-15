#include "StdAfx.h"
#include "ISTaskTodayListForm.h"
//-----------------------------------------------------------------------------
ISTaskTodayListForm::ISTaskTodayListForm(QWidget *parent) : ISTaskBaseListForm(parent)
{
	GetQueryModel()->SetClassFilter("task_deadline = CURRENT_DATE");
}
//-----------------------------------------------------------------------------
ISTaskTodayListForm::~ISTaskTodayListForm()
{

}
//-----------------------------------------------------------------------------
