#include "StdAfx.h"
#include "ISOrganizationListForm.h"
#include "ISDefines.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISUserEdit.h"
#include "ISTransferOrganizationForm.h"
#include "ISMessageBox.h"
#include "ISProtocol.h"
#include "ISDaDataService.h"
#include "ISProgressForm.h"
#include "ISNotifySender.h"
#include "ISObjects.h"
#include "ISInputDialog.h"
//-----------------------------------------------------------------------------
static QString QI_ORGANIZATION_WORK = PREPARE_QUERY("INSERT INTO organizationwork(orgw_user, orgw_organization, orgw_date) "
													"VALUES(:User, :Organization, :Date)");
//-----------------------------------------------------------------------------
static QString QU_ORGANIZATION_USER = PREPARE_QUERY("UPDATE organizations SET "
													"orgz_user = :UserID "
													"WHERE orgz_id = :OrganizationID")
//-----------------------------------------------------------------------------
static QString QU_RESET_EXECUTOR = PREPARE_QUERY("UPDATE organizations SET orgz_user = NULL WHERE orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
static QString QU_PERCENTAGE = PREPARE_QUERY("UPDATE organizations SET orgz_percentage = :Percentage WHERE orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
static QString QS_CALL_HISTORY = PREPARE_QUERY("SELECT DISTINCT clhs_organization "
											   "FROM callhistory "
											   "WHERE clhs_creationdate < :Date");
//-----------------------------------------------------------------------------
ISOrganizationListForm::ISOrganizationListForm(QWidget *parent) : ISListBaseForm("Organizations", parent)
{
	ISUserEdit *UserEdit = new ISUserEdit(GetToolBar());
	connect(UserEdit, &ISComboEdit::ValueChange, this, &ISOrganizationListForm::UserChanged);
	GetToolBar()->addWidget(UserEdit);

	QAction *ActionTransfer = new QAction(GetToolBar());
	ActionTransfer->setText(LANG("TransferInWork") + "...");
	ActionTransfer->setToolTip(LANG("TransferInWork"));
	ActionTransfer->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("TransferOrganization"));
	connect(ActionTransfer, &QAction::triggered, this, &ISOrganizationListForm::Transfer);
	AddAction(ActionTransfer, false, true);

	QAction *ActionResetExecutor = new QAction(GetToolBar());
	ActionResetExecutor->setText(LANG("ResetExecutor"));
	ActionResetExecutor->setToolTip(LANG("ResetExecutor"));
	ActionResetExecutor->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("ResetOrganizationExecutor"));
	connect(ActionResetExecutor, &QAction::triggered, this, &ISOrganizationListForm::ResetExecutor);
	AddAction(ActionResetExecutor, false, true);

	QAction *ActionPercentage = new QAction(this);
	ActionPercentage->setText(LANG("CalculatePercentageFill"));
	ActionPercentage->setToolTip(LANG("CalculatePercentageFill"));
	ActionPercentage->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Percentage"));
	connect(ActionPercentage, &QAction::triggered, this, &ISOrganizationListForm::Percentage);
	AddAction(ActionPercentage, false, true);

	QAction *ActionCalledMore = new QAction(this);
	ActionCalledMore->setText(LANG("InformResource.SelectDays"));
	ActionCalledMore->setToolTip(LANG("InformResource.SelectDays"));
	connect(ActionCalledMore, &QAction::triggered, this, &ISOrganizationListForm::CalledMoreDays);
	AddAction(ActionCalledMore, false, false);
}
//-----------------------------------------------------------------------------
ISOrganizationListForm::~ISOrganizationListForm()
{

}
//-----------------------------------------------------------------------------
void ISOrganizationListForm::UserChanged(const QVariant &value)
{
	if (value.isValid())
	{
		GetQueryModel()->SetClassFilter(QString("orgz_user = %1").arg(value.toInt()));
	}
	else
	{
		GetQueryModel()->ClearClassFilter();
	}

	Update();
}
//-----------------------------------------------------------------------------
void ISOrganizationListForm::Transfer()
{
	QVectorInt Objects = GetSelectedIDs();
	if (Objects.count())
	{
		ISTransferOrganizationForm TransferOrganizationForm(Objects.count());
		if (TransferOrganizationForm.Exec())
		{
			for (int i = 0; i < Objects.count(); ++i)
			{
				int ObjectID = Objects.at(i);
				QString OrganizationName = GetRecordValue("Name", GetRowIndex(ObjectID)).toString();
				int UserID = TransferOrganizationForm.GetSelectedUserID();

				if (UserID)
				{
					ISQuery qUpdate(QI_ORGANIZATION_WORK);
					qUpdate.BindValue(":User", UserID);
					qUpdate.BindValue(":Organization", ObjectID);
					qUpdate.BindValue(":Date", TransferOrganizationForm.GetSelectedDate());
					if (qUpdate.Execute())
					{
						ISQuery qUpdateOrgUser(QU_ORGANIZATION_USER);
						qUpdateOrgUser.BindValue(":UserID", UserID);
						qUpdateOrgUser.BindValue(":OrganizationID", ObjectID);
						qUpdateOrgUser.Execute();

						ISProtocol::Insert(true, "{BB6C46A0-C8E4-48F7-AC19-3A31D8FE7888}", GetMetaTable()->GetName(), GetMetaTable()->GetLocalListName(), ObjectID, LANG("Protocol.Transfer").arg(OrganizationName).arg(TransferOrganizationForm.GetSelectedUserName()));
						ISNotifySender::GetInstance().SendToUser("{F436CA43-860B-4728-83E1-0F38A05A6282}", UserID, OrganizationName);
					}
				}
				else
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotSelectedUserWithTransfer"));
					return;
				}
			}

			ISMessageBox::ShowInformation(this, LANG("Message.Information.TrasferOrganizationUserDone").arg(Objects.count()).arg(TransferOrganizationForm.GetSelectedUserName()));
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
void ISOrganizationListForm::ResetExecutor()
{
	QVectorInt Objects = GetSelectedIDs();
	if (Objects.count())
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ResetOrganizationExecutor").arg(Objects.count())))
		{
			for (int i = 0; i < Objects.count(); ++i)
			{
				ISQuery qUpdate(QU_RESET_EXECUTOR);
				qUpdate.BindValue(":OrganizationID", Objects.at(i));
				qUpdate.Execute();
			}

			ISMessageBox::ShowInformation(this, LANG("Message.Information.ResetOrganizationExecutor").arg(Objects.count()));
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
void ISOrganizationListForm::Percentage()
{
	QVectorInt Objects = GetSelectedIDs();
	if (Objects.count())
	{
		ISProgressForm ProgressForm(0, Objects.count(), this);
		ProgressForm.show();

		for (int i = 0; i < Objects.count(); ++i) //Обход всех организаций
		{
			ProgressForm.SetText(LANG("CalculatePercentage").arg(i).arg(Objects.count()));
			ProgressForm.setValue(i);

			int CountFill = 0; //Количество заполненных полей
			int CountFields = GetMetaTable()->GetFields().count(); //Количество полей

			for (int j = 0; j < CountFields; ++j) //Обход полей
			{
				PMetaClassField *MetaField = GetMetaTable()->GetFields().at(j);
				QString FieldName = "orgz_" + MetaField->GetName();

				ISQuery qSelectField(QString("SELECT %1 FROM organizations WHERE orgz_id = :OrganizationID").arg(FieldName));
				qSelectField.BindValue(":OrganizationID", Objects.at(i));
				if (qSelectField.ExecuteFirst())
				{
					QVariant Value = qSelectField.ReadColumn(FieldName);
					if (!Value.isNull())
					{
						CountFill++;
					}
				}
			}

			double Percent = CountFill * 100 / CountFields;

			ISQuery qUpdatePercentage(QU_PERCENTAGE);
			qUpdatePercentage.BindValue(":Percentage", Percent);
			qUpdatePercentage.BindValue(":OrganizationID", Objects.at(i));
			qUpdatePercentage.Execute();

			if (ProgressForm.wasCanceled())
			{
				return;
			}
		}

		ProgressForm.close();
		ISMessageBox::ShowInformation(this, LANG("Message.Information.CalculatePercantageOrganizations"));
		Update();
	}
}
//-----------------------------------------------------------------------------
void ISOrganizationListForm::CalledMoreDays()
{
	QVariant Days = ISInputDialog::GetInteger(this, LANG("Days"), LANG("InformResource.InputCountDays") + ':');
	if (Days.isValid())
	{
		if (Days.toInt() > 0)
		{
			GetQueryModel()->SetClassFilter("orgz.orgz_id IN (WITH t AS(SELECT orgz_id, (SELECT MAX(clhs_creationdate) FROM callhistory WHERE clhs_organization = orgz_id) FROM organizations ) SELECT orgz_id FROM t WHERE max < (SELECT (CURRENT_DATE - INTEGER '" + Days.toString() + "')) ORDER BY orgz_id)");
		}
		else
		{
			GetQueryModel()->ClearClassFilter();
		}
		
		Update();
	}
}
//-----------------------------------------------------------------------------
