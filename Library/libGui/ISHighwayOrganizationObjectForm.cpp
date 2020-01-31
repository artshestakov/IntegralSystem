#include "StdAfx.h"
#include "ISHighwayOrganizationObjectForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISSystem.h"
#include "ISQuery.h"
#include "ISInputDialog.h"
#include "ISGui.h"
#include "ISObjects.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QS_PARENT = PREPARE_QUERY("SELECT o.orgz_parent, (SELECT orgz_name FROM organization WHERE orgz_id = o.orgz_parent) "
										 "FROM organization o "
										 "WHERE o.orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
static QString QS_PARENTS = PREPARE_QUERY("SELECT orgz_id, orgz_name "
										  "FROM organization "
										  "WHERE NOT orgz_isdeleted "
										  "AND orgz_parent = :ParentID");
//-----------------------------------------------------------------------------
static QString QS_ORGANIZATION = PREPARE_QUERY("SELECT orgz_id, orgz_name "
											   "FROM organization "
											   "WHERE NOT orgz_isdeleted "
											   "AND orgz_parent IS NULL "
											   "AND orgz_id != :CurrentOrganization "
											   "AND orgz_parent IS NULL "
											   "ORDER BY orgz_name");
//-----------------------------------------------------------------------------
static QString QU_ORGANIZATION_BRANCH = PREPARE_QUERY("UPDATE organization SET "
													  "orgz_parent = :Parent "
													  "WHERE orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
static QString QU_REBIND = PREPARE_QUERY("UPDATE organization SET orgz_parent = NULL WHERE orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
ISHighwayOrganizationObjectForm::ISHighwayOrganizationObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	ActionBranchMenu = new QAction(this);
	ActionBranchMenu->setText(LANG("Highway.Branches").arg(0));
	ActionBranchMenu->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Branch"));
	ActionBranchMenu->setMenu(new QMenu(this));
	AddActionToolBar(ActionBranchMenu);

	QToolButton *ToolButton = dynamic_cast<QToolButton*>(GetToolBar()->widgetForAction(ActionBranchMenu));
	ToolButton->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
	ToolButton->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));
}
//-----------------------------------------------------------------------------
ISHighwayOrganizationObjectForm::~ISHighwayOrganizationObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISHighwayOrganizationObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();

	ISQuery qSelect(QS_PARENT);
	qSelect.BindValue(":OrganizationID", GetObjectID());
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("orgz_parent").toInt()) //Если у организации нет родителя
		{
			ActionBranchMenu->setVisible(false);

			ActionRebind = new QAction(this);
			ActionRebind->setText(LANG("Highway.Branches.Rebind").arg(qSelect.ReadColumn("orgz_name").toString()));
			connect(ActionRebind, &QAction::triggered, this, &ISHighwayOrganizationObjectForm::RebindBranch);
			AddActionToolBar(ActionRebind);
		}
		else
		{
			if (GetFormType() == ISNamespace::OFT_Edit)
			{
				ActionBranchMenu->setVisible(true);
				ReloadBranch();
			}
			else
			{
				ActionBranchMenu->setVisible(false);
			}
		}
	}
}
//-----------------------------------------------------------------------------
bool ISHighwayOrganizationObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result)
	{
		ActionBranchMenu->setVisible(true);
		ReloadBranch();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISHighwayOrganizationObjectForm::BranchClicked()
{
	ISHighwayOrganizationObjectForm *HighwayOrganizationObjectForm = dynamic_cast<ISHighwayOrganizationObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, GetMetaTable()->GetName(), dynamic_cast<QAction*>(sender())->data().toInt()));
	connect(HighwayOrganizationObjectForm, &ISHighwayOrganizationObjectForm::UpdateList, this, &ISHighwayOrganizationObjectForm::ReloadBranch);
	HighwayOrganizationObjectForm->show();
}
//-----------------------------------------------------------------------------
void ISHighwayOrganizationObjectForm::BindBranch()
{
	ISGui::SetWaitGlobalCursor(true);

	QVariantMap VariantMap;
	ISQuery qSelect(QS_ORGANIZATION);
	qSelect.BindValue(":CurrentOrganization", GetObjectID());
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			VariantMap.insert(qSelect.ReadColumn("orgz_name").toString(), qSelect.ReadColumn("orgz_id"));
		}
	}

	ISGui::SetWaitGlobalCursor(false);
	QVariant Value = ISInputDialog::GetList(this, LANG("Highway.Branche"), LANG("Highway.SelectBranche") + ":", VariantMap);
	if (Value.isValid())
	{
		ISQuery qUpdateBranch(QU_ORGANIZATION_BRANCH);
		qUpdateBranch.BindValue(":Parent", GetObjectID());
		qUpdateBranch.BindValue(":OrganizationID", Value);
		if (qUpdateBranch.Execute())
		{
			ReloadBranch();
			emit UpdateList();
		}
	}
}
//-----------------------------------------------------------------------------
void ISHighwayOrganizationObjectForm::RebindBranch()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Highway.Message.Question.Rebind").arg(GetObjectName())))
	{
		ISQuery qRebind(QU_REBIND);
		qRebind.BindValue(":OrganizationID", GetObjectID());
		if (qRebind.Execute())
		{
			ActionRebind->setVisible(false);
			emit UpdateList();
		}
	}
}
//-----------------------------------------------------------------------------
void ISHighwayOrganizationObjectForm::ReloadBranch()
{
	while (ActionBranchMenu->menu()->actions().count())
	{
		QAction *Action = ActionBranchMenu->menu()->actions().takeFirst();
		ActionBranchMenu->menu()->removeAction(Action);
	}

	ISQuery qSelectParent(QS_PARENTS);
	qSelectParent.BindValue(":ParentID", GetObjectID());
	if (qSelectParent.Execute())
	{
		if (qSelectParent.GetCountResultRows())
		{
			while (qSelectParent.Next())
			{
				QAction *Action = new QAction(this);
				Action->setText(qSelectParent.ReadColumn("orgz_name").toString());
				Action->setData(qSelectParent.ReadColumn("orgz_id"));
				connect(Action, &QAction::triggered, this, &ISHighwayOrganizationObjectForm::BranchClicked);
				ActionBranchMenu->menu()->addAction(Action);
			}
			ActionBranchMenu->menu()->addSeparator();
			ActionBranchMenu->setText(LANG("Highway.Branches") + " (" + QString::number(qSelectParent.GetCountResultRows()) + ")");
		}
		else
		{
			ActionBranchMenu->setText(LANG("Highway.Branches").arg(0));
		}
	}

	ActionBranchMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Highway.Branches.Bind"), this, &ISHighwayOrganizationObjectForm::BindBranch);
}
//-----------------------------------------------------------------------------
