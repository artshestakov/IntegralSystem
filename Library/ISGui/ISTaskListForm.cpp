#include "ISTaskListForm.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISTaskListForm::ISTaskListForm(QWidget *parent) : ISListBaseForm("_Task", parent)
{

}
//-----------------------------------------------------------------------------
ISTaskListForm::~ISTaskListForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskListForm::Edit()
{
	ISGui::ShowTaskViewForm(GetObjectID());
}
//-----------------------------------------------------------------------------
