#include "StdAfx.h"
#include "ISTaskOverdueListForm.h"
#include "EXConstants.h"
//-----------------------------------------------------------------------------
ISTaskOverdueListForm::ISTaskOverdueListForm(QWidget *parent) : ISTaskBaseListForm(parent)
{
	GetQueryModel()->SetClassFilter(QString("task_status = (SELECT tsst_id FROM _taskstatus WHERE tsst_uid = '%1') AND task_deadline < CURRENT_DATE").arg(CONST_UID_TASK_STATUS_IN_WORK));
}
//-----------------------------------------------------------------------------
ISTaskOverdueListForm::~ISTaskOverdueListForm()
{

}
//-----------------------------------------------------------------------------
