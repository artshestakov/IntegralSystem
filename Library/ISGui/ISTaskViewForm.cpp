#include "ISTaskViewForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISTaskViewForm::ISTaskViewForm(int task_id, QWidget *parent)
	: ISInterfaceForm(parent),
	TaskID(task_id)
{
	setWindowTitle(LANG("Task.CardTask") + ": ");
	setWindowIcon(BUFFER_ICONS("Task"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
}
//-----------------------------------------------------------------------------
ISTaskViewForm::~ISTaskViewForm()
{

}
//-----------------------------------------------------------------------------
