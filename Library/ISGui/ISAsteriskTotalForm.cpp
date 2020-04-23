#include "ISAsteriskTotalForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISButtons.h"
#include "ISGui.h"
#include "ISTelephonySummary.h"
//-----------------------------------------------------------------------------
static QString QS_ASTERISK_PATTER = PREPARE_QUERY("SELECT userfullname(aspt_user), aspt_pattern "
												  "FROM _asteriskpattern "
												  "WHERE NOT aspt_isdeleted "
												  "ORDER BY userfullname(aspt_user)");
//-----------------------------------------------------------------------------
static QString QS_PATTER_COUNT_OUTCOMING = PREPARE_QUERY("SELECT COUNT(*) "
														 "FROM _asteriskcalls "
														 "WHERE ascl_dialbegin BETWEEN :DateBegin AND :DateEnd "
														 "AND ascl_direction = (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = '{3E1A44EB-74FD-48A2-A472-FB5B0225DAE9}') "
														 "AND ascl_subscriber = :Pattern "
														 "AND ascl_dialstatus = (SELECT asds_id FROM _asteriskcallstatus WHERE asds_uid = '{6DA83A49-93D3-4D5F-AFC9-C5C89C82E894}')");
//-----------------------------------------------------------------------------
static QString QS_PATTER_COUNT_INCOMING = PREPARE_QUERY("SELECT COUNT(*) "
														"FROM _asteriskcalls "
														"WHERE ascl_dialbegin BETWEEN :DateBegin AND :DateEnd "
														"AND ascl_direction = (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = '{EB1D0FC1-6F56-406F-9819-9C71990998EC}') "
														"AND ascl_number = :Pattern "
														"AND ascl_dialstatus = (SELECT asds_id FROM _asteriskcallstatus WHERE asds_uid = '{6DA83A49-93D3-4D5F-AFC9-C5C89C82E894}')");
//-----------------------------------------------------------------------------
static QString QS_DURATION_OUTCOMING = PREPARE_QUERY("SELECT to_char(((SELECT SUM(ascl_duration) "
													 "FROM _asteriskcalls "
													 "WHERE ascl_duration IS NOT NULL "
													 "AND ascl_direction = (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = '{3E1A44EB-74FD-48A2-A472-FB5B0225DAE9}') "
													 "AND ascl_dialbegin BETWEEN :DateBegin AND :DateEnd "
													 "AND ascl_dialstatus = (SELECT asds_id FROM _asteriskcallstatus WHERE asds_uid = '{6DA83A49-93D3-4D5F-AFC9-C5C89C82E894}')"
													 "AND ascl_subscriber = :Pattern) || ' second')::INTERVAL, 'HH24:MI:SS')");
//-----------------------------------------------------------------------------
static QString QS_DURATION_INCOMING = PREPARE_QUERY("SELECT to_char(((SELECT SUM(ascl_duration) "
													"FROM _asteriskcalls "
													"WHERE ascl_duration IS NOT NULL "
													"AND ascl_direction = (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = '{EB1D0FC1-6F56-406F-9819-9C71990998EC}') "
													"AND ascl_dialbegin BETWEEN :DateBegin AND :DateEnd "
													"AND ascl_dialstatus = (SELECT asds_id FROM _asteriskcallstatus WHERE asds_uid = '{6DA83A49-93D3-4D5F-AFC9-C5C89C82E894}')"
													"AND ascl_number = :Pattern) || ' second')::INTERVAL, 'HH24:MI:SS')");
//-----------------------------------------------------------------------------
ISAsteriskTotalForm::ISAsteriskTotalForm(QWidget *parent) : ISInterfaceForm(parent)
{
	GetMainLayout()->addWidget(new QLabel(LANG("ClickDoneFromFilter"), this));

	QHBoxLayout *Layout = new QHBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *Widget = new QWidget(this);
	Widget->setLayout(Layout);
	Widget->setSizePolicy(Widget->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	GetMainLayout()->addWidget(Widget);

	RangeDateTimeEdit = new ISRangeDateTimeEdit(this, LANG("Period") + ':');
	RangeDateTimeEdit->SetValue(ISRangeStruct(QDateTime(QDate::currentDate(), QTime(0, 0)), QDateTime::currentDateTime()));
	Layout->addWidget(RangeDateTimeEdit);

	ButtonRange = new ISPushButton(this);
	ButtonRange->setText(LANG("Apply"));
	ButtonRange->setIcon(BUFFER_ICONS("Apply.Blue"));
	connect(ButtonRange, &ISPushButton::clicked, this, &ISAsteriskTotalForm::Update);
	Layout->addWidget(ButtonRange);

	Layout->addStretch();

	CreateTotal();
	CreateUsers();
}
//-----------------------------------------------------------------------------
ISAsteriskTotalForm::~ISAsteriskTotalForm()
{

}
//-----------------------------------------------------------------------------
void ISAsteriskTotalForm::CreateTotal()
{
	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	LabelCountTotal = new QLabel(this);
	FormLayout->addRow(LANG("TelephonyForm.Count.Total") + ':', LabelCountTotal);

	LabelCountIncoming = new QLabel(this);
	FormLayout->addRow(LANG("TelephonyForm.Count.Incoming") + ':', LabelCountIncoming);

	LabelCountOutcoming = new QLabel(this);
	FormLayout->addRow(LANG("TelephonyForm.Count.Outcoming") + ':', LabelCountOutcoming);

	LabelDurationTotal = new QLabel(this);
	FormLayout->addRow(LANG("TelephonyForm.Duration.Total") + ':', LabelDurationTotal);

	LabelDurationIncoming = new QLabel(this);
	FormLayout->addRow(LANG("TelephonyForm.Duration.Incoming") + ':', LabelDurationIncoming);

	LabelDurationOutcoming = new QLabel(this);
	FormLayout->addRow(LANG("TelephonyForm.Duration.Outcoming") + ':', LabelDurationOutcoming);

	LabelAverageIncoming = new QLabel(this);
	FormLayout->addRow(LANG("TelephonyForm.Average.Incoming") + ':', LabelAverageIncoming);

	LabelAverageOutcoming = new QLabel(this);
	FormLayout->addRow(LANG("TelephonyForm.Average.Outcoming") + ':', LabelAverageOutcoming);
}
//-----------------------------------------------------------------------------
void ISAsteriskTotalForm::CreateUsers()
{
	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LANG("TelephonyForm.Summary.Users"));
	GroupBox->setLayout(new QVBoxLayout());
	GetMainLayout()->addWidget(GroupBox);

	TreeWidget = new QTreeWidget(this);
	TreeWidget->setAlternatingRowColors(true);
	TreeWidget->setSortingEnabled(true);
	TreeWidget->setRootIsDecorated(false);
	GroupBox->layout()->addWidget(TreeWidget);

	QStringList StringList;
	StringList.append(LANG("User"));
	StringList.append(LANG("TelephonyForm.Pattern"));
	StringList.append(LANG("TelephonyForm.Count.Outcoming"));
	StringList.append(LANG("TelephonyForm.Count.Incoming"));

	StringList.append(LANG("TelephonyForm.Duration.Outcoming"));
	StringList.append(LANG("TelephonyForm.Duration.Incoming"));
	TreeWidget->setHeaderLabels(StringList);
}
//-----------------------------------------------------------------------------
void ISAsteriskTotalForm::Update()
{
	ISGui::SetWaitGlobalCursor(true);

	ButtonRange->setText(LANG("TelephonyForm.Summary.Requery"));
	ButtonRange->setIcon(BUFFER_ICONS("LoadData"));
	ISGui::RepaintWidget(ButtonRange);

	ReloadTotal();
	ReloadUsers();

	ButtonRange->setText(LANG("Apply"));
	ButtonRange->setIcon(BUFFER_ICONS("Apply.Blue"));

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISAsteriskTotalForm::ReloadTotal()
{
	int Incoming = ISTelephonySummary::GetCountIncoming(RangeDateTimeEdit->GetValue());
	int Outcoming = ISTelephonySummary::GetCountOutcoming(RangeDateTimeEdit->GetValue());

	LabelCountTotal->setText(QString::number(Incoming + Outcoming));
	LabelCountIncoming->setText(QString::number(Incoming));
	LabelCountOutcoming->setText(QString::number(Outcoming));

	LabelDurationTotal->setText(ISTelephonySummary::GetDurationAll(RangeDateTimeEdit->GetValue()));
	LabelDurationIncoming->setText(ISTelephonySummary::GetDurationIncoming(RangeDateTimeEdit->GetValue()));
	LabelDurationOutcoming->setText(ISTelephonySummary::GetDurationOutcoming(RangeDateTimeEdit->GetValue()));

	LabelAverageIncoming->setText(ISTelephonySummary::GetAverageIncoming(RangeDateTimeEdit->GetValue()));
	LabelAverageOutcoming->setText(ISTelephonySummary::GetAverageOutcoming(RangeDateTimeEdit->GetValue()));
}
//-----------------------------------------------------------------------------
void ISAsteriskTotalForm::ReloadUsers()
{
	while (TreeWidget->topLevelItemCount())
	{
		QTreeWidgetItem *TreeWidgetItem = TreeWidget->takeTopLevelItem(0);
		if (TreeWidgetItem)
		{
			delete TreeWidgetItem;
			TreeWidgetItem = nullptr;
		}
	}

	ISQuery qSelect(QS_ASTERISK_PATTER);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString UserName = qSelect.ReadColumn("userfullname").toString();
			int Pattern = qSelect.ReadColumn("aspt_pattern").toInt();

			QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem(TreeWidget);
			TreeWidgetItem->setText(0, UserName);
			TreeWidgetItem->setText(1, QString::number(Pattern));
			TreeWidgetItem->setSizeHint(0, QSize(TreeWidgetItem->sizeHint(0).width(), 30));

			ISQuery qSelectCountOut(QS_PATTER_COUNT_OUTCOMING);
			qSelectCountOut.BindValue(":DateBegin", RangeDateTimeEdit->GetValue().BeginValue);
			qSelectCountOut.BindValue(":DateEnd", RangeDateTimeEdit->GetValue().EndValue);
			qSelectCountOut.BindValue(":Pattern", Pattern);
			if (qSelectCountOut.ExecuteFirst())
			{
				int c = qSelectCountOut.ReadColumn("count").toInt();
				TreeWidgetItem->setText(2, QString::number(c));
			}

			ISQuery qSelectCountIn(QS_PATTER_COUNT_INCOMING);
			qSelectCountIn.BindValue(":DateBegin", RangeDateTimeEdit->GetValue().BeginValue);
			qSelectCountIn.BindValue(":DateEnd", RangeDateTimeEdit->GetValue().EndValue);
			qSelectCountIn.BindValue(":Pattern", Pattern);
			if (qSelectCountIn.ExecuteFirst())
			{
				int c = qSelectCountIn.ReadColumn("count").toInt();
				TreeWidgetItem->setText(3, QString::number(c));
			}

			ISQuery qSelectDurationOut(QS_DURATION_OUTCOMING);
			qSelectDurationOut.BindValue(":DateBegin", RangeDateTimeEdit->GetValue().BeginValue);
			qSelectDurationOut.BindValue(":DateEnd", RangeDateTimeEdit->GetValue().EndValue);
			qSelectDurationOut.BindValue(":Pattern", Pattern);
			if (qSelectDurationOut.ExecuteFirst())
			{
				TreeWidgetItem->setText(4, qSelectDurationOut.ReadColumn("to_char").toString());
			}

			ISQuery qSelectDurationIn(QS_DURATION_INCOMING);
			qSelectDurationIn.BindValue(":DateBegin", RangeDateTimeEdit->GetValue().BeginValue);
			qSelectDurationIn.BindValue(":DateEnd", RangeDateTimeEdit->GetValue().EndValue);
			qSelectDurationIn.BindValue(":Pattern", Pattern);
			if (qSelectDurationIn.ExecuteFirst())
			{
				TreeWidgetItem->setText(5, qSelectDurationIn.ReadColumn("to_char").toString());
			}

			TreeWidgetItem->setTextAlignment(1, Qt::AlignCenter);
			TreeWidgetItem->setTextAlignment(2, Qt::AlignCenter);
			TreeWidgetItem->setTextAlignment(3, Qt::AlignCenter);
			TreeWidgetItem->setTextAlignment(4, Qt::AlignCenter);
		}
	}

	TreeWidget->sortItems(0, Qt::AscendingOrder);
	TreeWidget->resizeColumnToContents(0);
	TreeWidget->resizeColumnToContents(1);
	TreeWidget->resizeColumnToContents(2);
	TreeWidget->resizeColumnToContents(3);
	TreeWidget->resizeColumnToContents(4);
}
//-----------------------------------------------------------------------------
