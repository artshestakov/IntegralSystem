#include "StdAfx.h"
#include "ISTaskBaseListForm.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISCore.h"
#include "ISMetaUser.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISTaskBaseListForm::ISTaskBaseListForm(QWidget *parent) : ISListBaseForm("_Task", parent)
{
	ActionTakeToWork = new QAction(this);
	ActionTakeToWork->setEnabled(false);
	ActionTakeToWork->setText(LOCALIZATION("Task.TakeToWork"));
	ActionTakeToWork->setToolTip(LOCALIZATION("Task.TakeToWork"));
	ActionTakeToWork->setIcon(BUFFER_ICONS("Task.TakeToWork"));
	connect(ActionTakeToWork, &QAction::triggered, this, &ISTaskBaseListForm::TakeToWork);
	AddAction(ActionTakeToWork, true, true);
}
//-----------------------------------------------------------------------------
ISTaskBaseListForm::~ISTaskBaseListForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskBaseListForm::Edit()
{
	ISUuid StatusUID = ISCore::TaskGetStatusUID(GetObjectID());
	if (StatusUID == CONST_UID_TASK_STATUS_NEW)
	{
		ISListBaseForm::Edit();
	}
	else
	{
		ISGui::ShowTaskViewForm(GetObjectID());
	}
}
//-----------------------------------------------------------------------------
void ISTaskBaseListForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
	Edit();
}
//-----------------------------------------------------------------------------
void ISTaskBaseListForm::TakeToWork()
{
	if (GetCurrentRecordValueDB("Executor").toInt()) //���� � ������ ���� �����������
	{
		if (ISCore::TaskGetStatusUID(GetObjectID()) == CONST_UID_TASK_STATUS_IN_WORK) //���� ������ ��� � ������
		{
			if (GetCurrentRecordValueDB("Executor").toInt() == ISMetaUser::GetInstance().GetData()->ID) //���� ������ ��� � ������ � �������� ������������
			{
				ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.TakeAlreadyInWorkYou"));
			}
			else //������ ��� � ������ � ������� ������������
			{
				ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.TakeAlreadyInWorkUser").arg(GetCurrentRecordValue("Executor").toString()));
			}
		}
		else //������ �� � ������
		{
			if (GetCurrentRecordValueDB("Executor").toInt() == ISMetaUser::GetInstance().GetData()->ID) //���� ������������ ������ �������� ������� ������������
			{
				if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.TaskTakeToWork")))
				{
					ISCore::TaskSetStatus(GetObjectID(), CONST_UID_TASK_STATUS_IN_WORK);
					SetSelectObjectAfterUpdate(GetObjectID());
					Update();
					ISGui::ShowTaskViewForm(GetObjectID());
				}
			}
			else //������������ ������ �������� �� ������� ������������
			{
				ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.AlienTask"));
			}
		}
	}
	else //� ������ ����������� �����������
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.TaskExecutorEmpty"));
	}
}
//-----------------------------------------------------------------------------
