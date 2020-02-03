#include "ISDeviceListForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISInputDialog.h"
#include "ISCore.h"
#include "ISMessageBox.h"
#include "ISDefines.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
static QString QS_PHONE_NUMBER = PREPARE_QUERY("SELECT c.clts_phonemobile FROM clients c WHERE c.clts_id = (SELECT d.dvce_client FROM device d WHERE NOT d.dvce_isdeleted AND d.dvce_id = :DeviceID)");
//-----------------------------------------------------------------------------
static QString QS_TOTAL_SUM_SERVICES = PREPARE_QUERY("SELECT sum(s.srcs_cost * s.srcs_count) FROM services s WHERE NOT s.srcs_isdeleted AND s.srcs_device = :DeviceID");
//-----------------------------------------------------------------------------
static QString QS_TOTAL_SUM_USED_PARTS = PREPARE_QUERY("SELECT sum(up.uacs_cost * up.uacs_count) FROM usedparts up WHERE NOT up.uacs_isdeleted AND up.uacs_device = :DeviceID");
//-----------------------------------------------------------------------------
static QString QU_ISSUE = PREPARE_QUERY("UPDATE device SET dvce_DateIssue = now(), dvce_StageRepair = (SELECT sgrp_id FROM StageRepairs WHERE sgrp_uid = :StageRepairUID), dvce_DiscoveredDefect = :Defect WHERE dvce_id = :DeviceID");
//-----------------------------------------------------------------------------
ISDeviceListForm::ISDeviceListForm(QWidget *parent) : ISListBaseForm("Device", parent)
{
	QAction *ActionIssue = new QAction(GetToolBar());
	ActionIssue->setToolTip(LANG("Issue"));
	ActionIssue->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("SubSystemIssued"));
	connect(ActionIssue, &QAction::triggered, this, &ISDeviceListForm::Issue);
	AddAction(ActionIssue);

	QLabel *LabelTotalSum = new QLabel(this);
	LabelTotalSum->setText(LANG("Math.Sum") + ':');
	AddWidgetToBottom(LabelTotalSum);

	DoubleEdit = new ISDoubleEdit(this);
	DoubleEdit->SetReadOnly(true);
	AddWidgetToBottom(DoubleEdit);
}
//-----------------------------------------------------------------------------
ISDeviceListForm::~ISDeviceListForm()
{

}
//-----------------------------------------------------------------------------
void ISDeviceListForm::Issue()
{
	QDate DateIssue = GetCurrentRecordValue("DateIssue").toDate();

	if (DateIssue.isNull())
	{
		bool Result = false;

		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.Issue")))
		{
			QString Defect = ISInputDialog::GetText(this, LANG("Defect"), LANG("EnterDefect") + ':').toString();

			ISQuery qUpdate(QU_ISSUE);

			if (Defect.length())
			{
				qUpdate.BindValue(":Defect", Defect);
			}
			else
			{
				qUpdate.BindValue(":Defect", QVariant());
			}

			qUpdate.BindValue(":StageRepairUID", QUuid("{7EE3B724-6011-455D-9A3C-27D6072933B3}"));
			qUpdate.BindValue(":DeviceID", GetObjectID());
			if (qUpdate.Execute())
			{
				Update();
				ISMessageBox::ShowInformation(this, LANG("Message.Information.Issued"));
			}
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.AlreadyIssued"));
	}
}
//-----------------------------------------------------------------------------
QString ISDeviceListForm::GetPhoneNumber()
{
	QString PhoneNumber = QString();

	ISQuery qSelectNumber(QS_PHONE_NUMBER);
	qSelectNumber.BindValue(":DeviceID", GetObjectID());
	if (qSelectNumber.ExecuteFirst())
	{
		PhoneNumber = qSelectNumber.ReadColumn("clts_phonemobile").toString();
	}

	return PhoneNumber;
}
//-----------------------------------------------------------------------------
int ISDeviceListForm::GetTotalSum()
{
	int TotalSum = 0;

	ISQuery qSelectServicesSum(QS_TOTAL_SUM_SERVICES);
	qSelectServicesSum.BindValue(":DeviceID", GetObjectID());
	if (qSelectServicesSum.ExecuteFirst())
	{
		TotalSum += qSelectServicesSum.ReadColumn("sum").toInt();
	}

	ISQuery qSelectUsedPartsSum(QS_TOTAL_SUM_USED_PARTS);
	qSelectUsedPartsSum.BindValue(":DeviceID", GetObjectID());
	if (qSelectUsedPartsSum.ExecuteFirst())
	{
		TotalSum += qSelectUsedPartsSum.ReadColumn("sum").toInt();
	}

	return TotalSum;
}
//-----------------------------------------------------------------------------
void ISDeviceListForm::Update()
{
	ISListBaseForm::Update();
	
	DoubleEdit->Clear();
	GetTableView()->setFocus();
}
//-----------------------------------------------------------------------------
void ISDeviceListForm::SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected)
{
	ISListBaseForm::SelectedRowEvent(ItemSelected, ItemDeSelected);

	int TotalSum = GetTotalSum();
	if (TotalSum)
	{
		DoubleEdit->SetValue(TotalSum);
	}
	else
	{
		DoubleEdit->Clear();
		GetTableView()->setFocus();
	}
}
//-----------------------------------------------------------------------------
