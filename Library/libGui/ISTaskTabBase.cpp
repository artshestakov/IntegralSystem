#include "StdAfx.h"
#include "ISTaskTabBase.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISTaskTabBase::ISTaskTabBase(int task_id, QTabWidget *parent) : QWidget(parent)
{
	TaskID = task_id;
	Count = 0;

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(MARGINS_LAYOUT_5_PX);
	setLayout(Layout);
}
//-----------------------------------------------------------------------------
ISTaskTabBase::~ISTaskTabBase()
{

}
//-----------------------------------------------------------------------------
int ISTaskTabBase::GetTaskID() const
{
	return TaskID;
}
//-----------------------------------------------------------------------------
void ISTaskTabBase::SetCount(int count)
{
	Count = count;
}
//-----------------------------------------------------------------------------
void ISTaskTabBase::Change()
{
	emit TabTextChanged(Count);
}
//-----------------------------------------------------------------------------
