#include "ISPatriotTrainerReportForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtons.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_TRAINERS = PREPARE_QUERY2("SELECT trnr_id, trnr_fio FROM trainer WHERE NOT trnr_isdeleted ORDER BY trnr_fio");
//-----------------------------------------------------------------------------
static QString QS_SUBSCRIPTIONS = PREPARE_QUERY2("SELECT SUM(sbsr_price) "
												"FROM subscriptions "
												"WHERE NOT sbsr_isdeleted "
												"AND sbsr_date BETWEEN :DateBegin AND :DateEnd");
//-----------------------------------------------------------------------------
static QString QS_COST = PREPARE_QUERY2("SELECT SUM(cost_sum) "
									   "FROM cost "
									   "WHERE NOT cost_isdeleted "
									   "AND cost_date BETWEEN :DateBegin AND :DateEnd");
//-----------------------------------------------------------------------------
ISPatriotTrainerReportForm::ISPatriotTrainerReportForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	QHBoxLayout *Layout = new QHBoxLayout();
	
	QWidget *Widget = new QWidget(this);
	Widget->setLayout(Layout);
	Widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	GetMainLayout()->addWidget(Widget);

	RangeDateEdit = new ISRangeDateEdit(this);
	Layout->addWidget(RangeDateEdit);

	Layout->addWidget(new QLabel(LANG("Patriot.Trainer") + ':', this));

	EditTrainer = new ISComboEdit(this);
	EditTrainer->SetEditable(false);
	EditTrainer->AddItem(LANG("NotSelected"), 0);
	Layout->addWidget(EditTrainer);

	ISQuery qSelectTrainer(QS_TRAINERS);
	if (qSelectTrainer.Execute())
	{
		while (qSelectTrainer.Next())
		{
			EditTrainer->AddItem(qSelectTrainer.ReadColumn("trnr_fio").toString(), qSelectTrainer.ReadColumn("trnr_id").toInt());
		}
	}

	ISPushButton *ButtonUpdate = new ISPushButton(Widget);
	ButtonUpdate->setText(LANG("Update"));
	ButtonUpdate->setIcon(BUFFER_ICONS("Update"));
	connect(ButtonUpdate, &ISPushButton::clicked, this, &ISPatriotTrainerReportForm::LoadData);
	Layout->addWidget(ButtonUpdate);

	Layout->addStretch();

	LabelReport = new QLabel(this);
	LabelReport->setText(LANG("Patriot.LabelReport").arg(0).arg(0));
	GetMainLayout()->addWidget(LabelReport, 0, Qt::AlignLeft);

	ListViewWidget = new ISListViewWidget(ISMetaData::GetInstanse().GetMetaQuery("TrainerReport"), this);
	ListViewWidget->setSizePolicy(ListViewWidget->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	GetMainLayout()->addWidget(ListViewWidget);
}
//-----------------------------------------------------------------------------
ISPatriotTrainerReportForm::~ISPatriotTrainerReportForm()
{

}
//-----------------------------------------------------------------------------
void ISPatriotTrainerReportForm::LoadData()
{
	double Subscriptions = 0;
	double Cost = 0;

	ISQuery qSelectSubscriptions(QS_SUBSCRIPTIONS);
	qSelectSubscriptions.BindValue(":DateBegin", RangeDateEdit->GetValue().BeginValue);
	qSelectSubscriptions.BindValue(":DateEnd", RangeDateEdit->GetValue().EndValue);
	if (qSelectSubscriptions.ExecuteFirst())
	{
		Subscriptions = qSelectSubscriptions.ReadColumn("sum").toDouble();
	}

	ISQuery qSelectCost(QS_COST);
	qSelectCost.BindValue(":DateBegin", RangeDateEdit->GetValue().BeginValue);
	qSelectCost.BindValue(":DateEnd", RangeDateEdit->GetValue().EndValue);
	if (qSelectCost.ExecuteFirst())
	{
		Cost = qSelectCost.ReadColumn("sum").toDouble();
	}

	LabelReport->setText(LANG("Patriot.LabelReport").arg(Subscriptions).arg(Cost));

	ListViewWidget->AddCondition(":TrainerID", EditTrainer->GetValue());
	ListViewWidget->AddCondition(":DateBegin", RangeDateEdit->GetValue().BeginValue);
	ListViewWidget->AddCondition(":DateEnd", RangeDateEdit->GetValue().EndValue);
	ListViewWidget->LoadData();
}
//-----------------------------------------------------------------------------
