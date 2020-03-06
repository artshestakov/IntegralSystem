#include "ISOrganizationObjectForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISMemoryObjects.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISInputDialog.h"
#include "ISPhoneEdit.h"
#include "ISDemoAccessesObjectForm.h"
#include "ISOrganizationScoreObjectForm.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
static QString QS_ORGANIZATION = PREPARE_QUERY2("SELECT orgz_id FROM organizations WHERE orgz_inn = :INN");
//-----------------------------------------------------------------------------
static QString QS_ORGANIZATION_USER = PREPARE_QUERY2("SELECT usrs_surname, usrs_name, usrs_patronymic FROM _users WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QI_CALL_HISTORY = PREPARE_QUERY2("INSERT INTO callhistory(clhs_organization, clhs_phone, clhs_result) "
											   "VALUES(:Organization, :Phone, (SELECT clrs_id FROM callresult WHERE clrs_uid = :ResultUID)) "
											   "RETURNING clhs_id");
//-----------------------------------------------------------------------------
static QString QS_OVED_COUNT = PREPARE_QUERY2("SELECT COUNT(*) FROM okved WHERE okvd_code = :Code");
//-----------------------------------------------------------------------------
static QString QS_OKVED = PREPARE_QUERY2("SELECT okvd_id FROM okved WHERE okvd_code = :Code");
//-----------------------------------------------------------------------------
static QString QS_DEMO_EXIST = PREPARE_QUERY2("SELECT COUNT(*) FROM demoaccesses WHERE NOT dacc_isdeleted AND dacc_organization = :ObjectID");
//-----------------------------------------------------------------------------
static QString QI_ORGANIZATION_WORK = PREPARE_QUERY2("INSERT INTO organizationwork(orgw_user, orgw_organization, orgw_date) "
													"VALUES(:User, :Organization, :Date)");
//-----------------------------------------------------------------------------
static QString QU_ORGANIZATION_USER = PREPARE_QUERY2("UPDATE organizations SET "
													"orgz_user = :UserID "
													"WHERE orgz_id = :OrganizationID");
//-----------------------------------------------------------------------------
ISOrganizationObjectForm::ISOrganizationObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	DaDataService = new ISDaDataService(this);
	connect(DaDataService, &ISDaDataService::FoundedOgranization, this, &ISOrganizationObjectForm::SearchFinished);

	for (int i = 0; i < meta_table->Fields.size(); ++i)
	{
		PMetaClassField *MetaField = meta_table->Fields[i];
		if (MetaField->Type == ISNamespace::FT_Phone)
		{
			ISPhoneEdit *PhoneEdit = dynamic_cast<ISPhoneEdit*>(GetFieldWidget(MetaField->Name));
			connect(PhoneEdit, &ISPhoneEdit::Called, this, &ISOrganizationObjectForm::Called);
		}
	}

	QAction *ActionNotify = new QAction(this);
	ActionNotify->setText(LANG("OrganizationNotify"));
	ActionNotify->setToolTip(LANG("OrganizationNotify"));
	ActionNotify->setIcon(BUFFER_ICONS("Bell"));
	connect(ActionNotify, &QAction::triggered, this, &ISOrganizationObjectForm::Notify);
	AddActionToolBar(ActionNotify);

	ActionDemo = new QAction(this);
	ActionDemo->setText(LANG("DemoAccess"));
	ActionDemo->setToolTip(LANG("DemoAccess"));
	connect(ActionDemo, &QAction::triggered, this, &ISOrganizationObjectForm::DemoAccess);
	AddActionToolBar(ActionDemo);

	ActionScore = new QAction(this);
	ActionScore->setText(LANG("Score"));
	ActionScore->setToolTip(LANG("Score"));
	connect(ActionScore, &QAction::triggered, this, &ISOrganizationObjectForm::Score);
	AddActionToolBar(ActionScore);

	UpdateDemo();

	QVariant UserID = GetFieldValue("User");
	if (UserID.isValid())
	{
		ISQuery qSelect(QS_ORGANIZATION_USER);
		qSelect.BindValue(":UserID", UserID);
		if (qSelect.ExecuteFirst())
		{
			QString UserName = qSelect.ReadColumn("usrs_surname").toString() + SYMBOL_SPACE + qSelect.ReadColumn("usrs_name").toString() + SYMBOL_SPACE + qSelect.ReadColumn("usrs_patronymic").toString();

			QLabel *LabelWorkIn = new QLabel(GetToolBar());
			LabelWorkIn->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
			LabelWorkIn->setText(LANG("OrganizationInWorkFromUser").arg(UserName));
			LabelWorkIn->setContentsMargins(10, 0, 0, 0);
			GetToolBar()->addWidget(LabelWorkIn);
		}
	}

	INNEdit = dynamic_cast<ISINNEdit*>(GetFieldWidget("INN"));
	connect(INNEdit, &ISINNEdit::ValueChange, this, &ISOrganizationObjectForm::INNChanged);
	connect(INNEdit, &ISINNEdit::SearchFromINN, this, &ISOrganizationObjectForm::SearchFromINN);
}
//-----------------------------------------------------------------------------
ISOrganizationObjectForm::~ISOrganizationObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOrganizationObjectForm::INNChanged(const QVariant &value)
{
	ISQuery qSelect(QS_ORGANIZATION);
	qSelect.BindValue(":INN", value);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.GetCountResultRows())
		{
			int OrganizationID = qSelect.ReadColumn("orgz_id").toInt();
			if (GetObjectID() == OrganizationID)
			{
				return;
			}

			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.FoundOrganizationCard").arg(value.toString())))
			{
				ISObjectFormBase *ObjectForm = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, GetMetaTable()->Name, OrganizationID);
				QMetaObject::invokeMethod(ISMemoryObjects::GetInstance().GetWorkspaceForm(), "AddObjectForm", Q_ARG(QWidget *, ObjectForm));
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISOrganizationObjectForm::Notify()
{
	QDateTime DateTime = ISInputDialog::GetDateTime(this, LANG("Reminder"), LANG("DateTime") + ':').toDateTime();
	if (DateTime.isValid())
	{
		if (ISGui::CalendarInsert(DateTime, LANG("CallToOrganization").arg(GetFieldWidget("Name")->GetValue().toString()), QVariant(), GetMetaTable()->Name, GetObjectID()))
		{
			ISQuery qUpdate(QI_ORGANIZATION_WORK);
			qUpdate.BindValue(":User", ISMetaUser::GetInstance().GetData()->ID);
			qUpdate.BindValue(":Organization", GetObjectID());
			qUpdate.BindValue(":Date", DateTime.date());
			if (qUpdate.Execute())
			{
				ISQuery qUpdateOrgUser(QU_ORGANIZATION_USER);
				qUpdateOrgUser.BindValue(":UserID", ISMetaUser::GetInstance().GetData()->ID);
				qUpdateOrgUser.BindValue(":OrganizationID", GetObjectID());
				qUpdateOrgUser.Execute();
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISOrganizationObjectForm::Score()
{
	ISOrganizationScoreObjectForm *OrganizationScoreObjectForm = dynamic_cast<ISOrganizationScoreObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_New, "OrganizationScore"));
	OrganizationScoreObjectForm->SetOrganization(GetObjectID());
	OrganizationScoreObjectForm->show();
}
//-----------------------------------------------------------------------------
void ISOrganizationObjectForm::DemoAccess()
{
	ISDemoAccessesObjectForm *DemoAccessesObjectForm = dynamic_cast<ISDemoAccessesObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_New, "DemoAccesses"));
	DemoAccessesObjectForm->SetOrganizationID(GetObjectID());
	connect(DemoAccessesObjectForm, &ISDemoAccessesObjectForm::UpdateList, this, &ISOrganizationObjectForm::UpdateDemo);
	DemoAccessesObjectForm->show();
}
//-----------------------------------------------------------------------------
void ISOrganizationObjectForm::Called()
{
	if (GetObjectID())
	{
		ISPhoneEdit *PhoneEdit = dynamic_cast<ISPhoneEdit*>(sender());

		ISQuery qInsertCallHisotory(QI_CALL_HISTORY);
		qInsertCallHisotory.BindValue(":Organization", GetObjectID());
		qInsertCallHisotory.BindValue(":Phone", PhoneEdit->GetValue());
		qInsertCallHisotory.BindValue(":ResultUID", "{5A92BB6B-9D51-4189-8BCE-787792964C99}"); //Результат - вызов абонента
		if (qInsertCallHisotory.ExecuteFirst())
		{
			ISObjectFormBase *CallHistoryObjectForm = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "CallHistory", qInsertCallHisotory.ReadColumn("clhs_id").toInt());
			CallHistoryObjectForm->SetParentObjectID(GetObjectID());
			CallHistoryObjectForm->show();
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SaveOrganizationCard"));
	}
}
//-----------------------------------------------------------------------------
void ISOrganizationObjectForm::UpdateDemo()
{
	if (GetFormType() == ISNamespace::OFT_Edit && GetObjectID())
	{
		ISQuery qSelectDemo(QS_DEMO_EXIST);
		qSelectDemo.BindValue(":ObjectID", GetObjectID());
		if (qSelectDemo.ExecuteFirst())
		{
			int CountDemo = qSelectDemo.ReadColumn("count").toInt();
			if (CountDemo)
			{
				ActionDemo->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
				ActionDemo->setText(LANG("DemoAccess") + " (" + QString::number(CountDemo) + ')');
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISOrganizationObjectForm::SearchFromINN()
{
	if (!INNEdit->GetValue().isValid())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.InvalidINN"));
		INNEdit->BlinkRed();
		return;
	}

	DaDataService->OrganizationFromINN(INNEdit->GetValue().toString());
}
//-----------------------------------------------------------------------------
void ISOrganizationObjectForm::SearchFinished(const ISDaDataOrganization &OrganizationStruct)
{
	if (!GetFieldWidget("Name")->GetValue().isValid())
	{
		GetFieldWidget("Name")->SetValue(OrganizationStruct.Name.ShortWithOPF);
	}

	if (!GetFieldWidget("KPP")->GetValue().isValid())
	{
		GetFieldWidget("KPP")->SetValue(OrganizationStruct.Kpp);
	}

	if (!GetFieldWidget("AddressPhysical")->GetValue().isValid())
	{
		GetFieldWidget("AddressPhysical")->SetValue(OrganizationStruct.Address);
	}

	if (!GetFieldWidget("AddressLegal")->GetValue().isValid())
	{
		GetFieldWidget("AddressLegal")->SetValue(OrganizationStruct.Address);
	}

	if (!GetFieldWidget("Principal")->GetValue().isValid())
	{
		GetFieldWidget("Principal")->SetValue(OrganizationStruct.Management.Name);
	}

	if (!OrganizationStruct.Okved.Okved.isEmpty())
	{
		ISQuery qSelectCount(QS_OVED_COUNT);
		qSelectCount.BindValue(":Code", OrganizationStruct.Okved.Okved);
		if (qSelectCount.ExecuteFirst())
		{
			if (qSelectCount.ReadColumn("count").toInt())
			{
				ISQuery qSelectOkved(QS_OKVED);
				qSelectOkved.BindValue(":Code", OrganizationStruct.Okved.Okved);
				if (qSelectOkved.ExecuteFirst())
				{
					QVariant OkvedID = qSelectOkved.ReadColumn("okvd_id").toInt();
					GetFieldWidget("Okved")->SetValue(OkvedID);
				}
			}
			else
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.OkvedNotFound").arg(GetFieldValue("INN").toString()));
				GetFieldWidget("INN")->BlinkRed();
			}
		}
	}
}
//-----------------------------------------------------------------------------
