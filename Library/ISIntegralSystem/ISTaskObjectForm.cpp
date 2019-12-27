#include "StdAfx.h"
#include "ISTaskObjectForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaUser.h"
#include "ISNotifySender.h"
#include "ISFavorites.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISCore.h"
#include "ISIntegralSystem.h"
//-----------------------------------------------------------------------------
static QString QS_TASK_STATUS = PREPARE_QUERY("SELECT tsst_id "
                                              "FROM _taskstatus "
                                              "WHERE tsst_uid = :StatusUID");
//-----------------------------------------------------------------------------
ISTaskObjectForm::ISTaskObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	CheckFavorite = nullptr;

	ActionTakeToWork = new QAction(this);
	ActionTakeToWork->setEnabled(false);
	ActionTakeToWork->setText(LOCALIZATION("Task.TakeToWork"));
	ActionTakeToWork->setToolTip(LOCALIZATION("Task.TakeToWork"));
	ActionTakeToWork->setIcon(BUFFER_ICONS("Task.TakeToWork"));
	connect(ActionTakeToWork, &QAction::triggered, this, &ISTaskObjectForm::TakeToWork);
	AddActionToolBar(ActionTakeToWork);
}
//-----------------------------------------------------------------------------
ISTaskObjectForm::~ISTaskObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();

	if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy)
	{
		CheckFavorite = new ISCheckEdit(this);
		CheckFavorite->SetText(LOCALIZATION("Task.Favorite"));
		CheckFavorite->SetToolTip(LOCALIZATION("Task.Favorite.ToolTip"));
		CheckFavorite->SetFont(FONT_APPLICATION_BOLD);
		CheckFavorite->setContentsMargins(5, 0, 0, 0);
		GetLayoutWidgetObject()->insertWidget(GetLayoutWidgetObject()->indexOf(GetToolBar()) + 2, CheckFavorite, 0, Qt::AlignLeft);

		if (GetFormType() == ISNamespace::OFT_Copy)
		{
			QString TaskName = GetFieldValue("Name").toString();
			TaskName += " - " + LOCALIZATION("Task.CloneTask.Copy");
			GetFieldWidget("Name")->SetValue(TaskName);
		}
	}
	else if (GetFormType() == ISNamespace::OFT_Edit)
	{
		ActionTakeToWork->setEnabled(true);
	}
}
//-----------------------------------------------------------------------------
bool ISTaskObjectForm::Save()
{
	if (GetFieldValue("Deadline").toDate().isValid()) //≈сли срок исполни€ валидный
	{
		if (GetFieldValue("Deadline").toDate() < DATE_TODAY) //≈сли дата срока исполнени€ меньше текущей даты
		{
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.TaskDeadlineOverdue"));
			GetFieldWidget("Deadline")->BlinkRed();
			return false;
		}
	}

	ISNamespace::ObjectFormType BeginFormType = GetFormType();
	if (BeginFormType == ISNamespace::OFT_New || BeginFormType == ISNamespace::OFT_Copy)
	{
		//”становка статуса задаче как "Ќова€"
		ISQuery qSelectStatus(QS_TASK_STATUS);
		qSelectStatus.BindValue(":StatusUID", CONST_UID_TASK_STATUS_NEW);
		if (qSelectStatus.ExecuteFirst())
		{
			GetFieldWidget("Status")->SetValue(qSelectStatus.ReadColumn("tsst_id").toInt());
		}
	}

	bool Result = ISObjectFormBase::Save();
	if (Result)
	{
		QString TaskName = GetFieldValue("Name").toString();
		int ExecutorID = GetFieldValue("Executor").toInt();

		if (ExecutorID && ExecutorID != CURRENT_USER_ID) //≈сли исполнитель не €вл€етс€ создателем - отправить уведомление
		{
			ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_TASK_CREATE_OR_UPDATE, ExecutorID, QVariant(), TaskName);
		}

		if (BeginFormType == ISNamespace::OFT_New || BeginFormType == ISNamespace::OFT_Copy)
		{
			if (CheckFavorite->GetValue().toBool())
			{
				ISFavorites::GetInstance().AddFavorite("_Task", LOCALIZATION("Tasks"), TaskName, GetObjectID());
			}

			CheckFavorite->setVisible(false);
		}

		ActionTakeToWork->setEnabled(true);
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISTaskObjectForm::TakeToWork()
{
	if (GetModificationFlag()) //≈сли карточка задачи изменена - предлжить сохранить
	{
		if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.SaveChangedBeforeTaskTakeToWork")))
		{
			Save();
		}
		else
		{
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.TaskSaveBeforeTakeToWork"));
			return;
		}
	}

	if (GetFieldValue("Executor").toInt()) //≈сли у задачи есть исполнитель
	{
		if (GetFieldValue("Executor").toInt() == CURRENT_USER_ID) //≈сли исполнителем задачи €вл€етс€ текущий пользователь
		{
			if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.TaskTakeToWork")))
			{
				ISCore::TaskSetStatus(GetObjectID(), CONST_UID_TASK_STATUS_IN_WORK);
				close();
				ISIntegralSystem::ShowTaskViewForm(GetObjectID());
			}
		}
		else //»сполнителем задачи €вл€етс€ не текущий пользователь
		{
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.AlienTask"));
			GetFieldWidget("Executor")->BlinkRed();
		}
	}
	else //” задачи отсутствует исполнитель
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.TaskExecutorEmpty"));
		GetFieldWidget("Executor")->BlinkRed();
	}
}
//-----------------------------------------------------------------------------
