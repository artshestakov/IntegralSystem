#include "ISTaskFromListForm.h"
//-----------------------------------------------------------------------------
ISTaskFromListForm::ISTaskFromListForm(QWidget *parent) : ISTaskBaseListForm(parent)
{
	GetQueryModel()->SetClassFilter("task_owner = currentuserid()");
}
//-----------------------------------------------------------------------------
ISTaskFromListForm::~ISTaskFromListForm()
{

}
//-----------------------------------------------------------------------------
