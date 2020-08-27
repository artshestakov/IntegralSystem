#include "ISAsteriskCallsListForm.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISControls.h"
#include "ISSettingsDatabase.h"
#include "ISFileDialog.h"
#include "ISProtocol.h"
#include "ISUserRoleEntity.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
static QString QS_CHILD_COUNT = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM _asteriskcalls "
											  "WHERE ascl_uniqueid = :UniqueID");
//-----------------------------------------------------------------------------
static QString QS_CALL = PREPARE_QUERY("SELECT ascl_dialbegin, asdr_name, ascl_subscriber, ascl_number, ascl_duration, asds_localname "
									   "FROM _asteriskcalls "
									   "LEFT JOIN _asteriskdirection ON asdr_id = ascl_direction "
									   "LEFT JOIN _asteriskcallstatus ON asds_id = ascl_dialstatus "
									   "WHERE ascl_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QS_ASTERISK_CALL_STATUSES = PREPARE_QUERY("SELECT asds_localname, asds_iconname, asds_description "
														 "FROM _asteriskcallstatus "
														 "WHERE NOT asds_isdeleted "
														 "ORDER BY asds_order");
//-----------------------------------------------------------------------------
static QString QS_ASTERISK_CALL_STATUS = PREPARE_QUERY("SELECT asds_uid "
													   "FROM _asteriskcalls "
													   "LEFT JOIN _asteriskcallstatus ON asds_id = ascl_dialstatus "
													   "WHERE ascl_id = :ObjectID");
//-----------------------------------------------------------------------------
ISAsteriskCallsListForm::ISAsteriskCallsListForm(QWidget *parent) : ISListBaseForm("_AsteriskCalls", parent)
{
	ObjectID = 0;
	DefaultFilter();

	ActionDetails = new QAction(this);
	ActionDetails->setText(LANG("CallDetails"));
	ActionDetails->setToolTip(LANG("CallDetails"));
	ActionDetails->setIcon(BUFFER_ICONS("Information"));
	connect(ActionDetails, &QAction::triggered, this, &ISAsteriskCallsListForm::DetailsClicked);
	AddAction(ActionDetails, true, true);

	CreateDetailsPanel();
	CreateStatusDescription();
}
//-----------------------------------------------------------------------------
ISAsteriskCallsListForm::~ISAsteriskCallsListForm()
{

}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::CreateDetailsPanel()
{
	WidgetCallInfo = new QWidget(this);
	WidgetCallInfo->setVisible(false);
	GetMainLayout()->insertWidget(1, WidgetCallInfo);

	QVBoxLayout *LayoutWidget = new QVBoxLayout();
	WidgetCallInfo->setLayout(LayoutWidget);

	QHBoxLayout *LayoutInfo = new QHBoxLayout();
	LayoutWidget->addLayout(LayoutInfo);

	ButtonBack = new ISPushButton(WidgetCallInfo);
	ButtonBack->setText(LANG("Back"));
	ButtonBack->setToolTip(LANG("ClickFromGoMainJournal"));
	ButtonBack->setIcon(BUFFER_ICONS("Arrow.Left"));
	ButtonBack->setSizePolicy(QSizePolicy::Maximum, ButtonBack->sizePolicy().verticalPolicy());
	connect(ButtonBack, &ISPushButton::clicked, this, &ISAsteriskCallsListForm::Back);
	LayoutInfo->addWidget(ButtonBack);

	QHBoxLayout *LayoutGroupBox = new QHBoxLayout();

	QGroupBox *GroupBox = new QGroupBox(WidgetCallInfo);
	GroupBox->setTitle(LANG("CallDetails"));
	GroupBox->setLayout(LayoutGroupBox);
	LayoutInfo->addWidget(GroupBox);

	LabelDateTime = new QLabel(GroupBox);
	LabelDateTime->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	LabelDirection = new QLabel(GroupBox);
	LabelDirection->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	LabelSubscriber = new QLabel(GroupBox);
	LabelSubscriber->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	LabelNumber = new QLabel(GroupBox);
	LabelNumber->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	LabelDuration = new QLabel(GroupBox);
	LabelDuration->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	LabelStatus = new QLabel(GroupBox);
	LabelStatus->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	QFormLayout *FormLayoutLeft = new QFormLayout();
	FormLayoutLeft->addRow(LANG("Call.DateTime") + ':', LabelDateTime);
	FormLayoutLeft->addRow(LANG("Call.Direction") + ':', LabelDirection);
	LayoutGroupBox->addLayout(FormLayoutLeft);

	LayoutGroupBox->addWidget(ISControls::CreateVerticalLine(GroupBox));

	QFormLayout *FormLayoutCentral = new QFormLayout();
	FormLayoutCentral->addRow(LANG("Call.Subscriber") + ':', LabelSubscriber);
	FormLayoutCentral->addRow(LANG("Call.Whom") + ':', LabelNumber);
	LayoutGroupBox->addLayout(FormLayoutCentral);

	LayoutGroupBox->addWidget(ISControls::CreateVerticalLine(GroupBox));

	QFormLayout *FormLayoutRight = new QFormLayout();
	FormLayoutRight->addRow(LANG("Call.Duration") + ':', LabelDuration);
	FormLayoutRight->addRow(LANG("Call.Status") + ':', LabelStatus);
	LayoutGroupBox->addLayout(FormLayoutRight);

	LayoutGroupBox->addStretch();

	LayoutWidget->addWidget(new QLabel(LANG("TransferCall") + ':', WidgetCallInfo));
}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::CreateStatusDescription()
{
	ISPushButton *ButtonStatus = new ISPushButton(this);
	ButtonStatus->setText(LANG("CallStatuses"));
	ButtonStatus->setIcon(BUFFER_ICONS("Apply.Green"));
	ButtonStatus->setMenu(new QMenu(ButtonStatus));
	GetStatusBar()->addWidget(ButtonStatus);

	ISQuery qSelect(QS_ASTERISK_CALL_STATUSES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QAction *ActionStatus = new QAction(ButtonStatus->menu());
			ActionStatus->setText(qSelect.ReadColumn("asds_localname").toString());
			ActionStatus->setIcon(BUFFER_ICONS(qSelect.ReadColumn("asds_iconname").toString()));
			ActionStatus->setData(qSelect.ReadColumn("asds_description").toString());
			connect(ActionStatus, &QAction::triggered, this, &ISAsteriskCallsListForm::StatusClicked);
			ButtonStatus->menu()->addAction(ActionStatus);
		}
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::DetailsClicked()
{
	DoubleClickedTable(QModelIndex());
}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::DoubleClickedTable(const QModelIndex &ModelIndex)
{
	ISListBaseForm::DoubleClickedTable(ModelIndex);
	QString UniqueID = GetCurrentRecordValueDB("UniqueID").toString();

	ISQuery qSelect(QS_CHILD_COUNT);
	qSelect.BindValue(":UniqueID", UniqueID);
	IS_ASSERT(qSelect.ExecuteFirst(), qSelect.GetErrorString());
	int ChildCount = qSelect.ReadColumn("count").toInt();

	if (ChildCount > 1)
	{
		ObjectID = GetObjectID();
		TransferFilter(UniqueID);

		ActionDetails->setVisible(false);
		WidgetCallInfo->setVisible(true);

		HideField("ID");
		HideField("Details");
		HideField("Subscriber");
		
		FillLabels();
		Update();
	}
	else
	{
		ISMessageBox::ShowInformation(this, LANG("Message.Information.NotFoundChildCalls"));
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::Back()
{
	SetSelectObjectAfterUpdate(ObjectID);
	
	ActionDetails->setVisible(true);
	WidgetCallInfo->setVisible(false);
	
	ShowField("ID");
	ShowField("Details");
	ShowField("Subscriber");

	DefaultFilter();
	Update();

	ObjectID = 0;

	LabelDateTime->clear();
	LabelDirection->clear();
	LabelSubscriber->clear();
	LabelNumber->clear();
	LabelDuration->clear();
	LabelStatus->clear();
}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::FillLabels()
{
	ISQuery qSelect(QS_CALL);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (qSelect.ExecuteFirst())
	{
		LabelDateTime->setText(qSelect.ReadColumn("ascl_dialbegin").toDateTime().toString(FORMAT_DATE_TIME_V3));
		LabelDirection->setText(qSelect.ReadColumn("asdr_name").toString());
		LabelSubscriber->setText(qSelect.ReadColumn("ascl_subscriber").toString());
		LabelNumber->setText(qSelect.ReadColumn("ascl_number").toString());
		LabelDuration->setText(QTime(0, 0).addSecs(qSelect.ReadColumn("ascl_duration").toInt()).toString(FORMAT_TIME_V3));
		LabelStatus->setText(qSelect.ReadColumn("asds_localname").toString());
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::StatusClicked()
{
	QAction *ActionStatus = dynamic_cast<QAction*>(sender());
	ISMessageBox::ShowInformation(this, ActionStatus->text() + ": " + ActionStatus->data().toString());
}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::DefaultFilter()
{
	GetQueryModel()->SetClassFilter(QString(
	"ascl.ascl_id IN "
	"( "
	"WITH uniq AS "
	"( "
	"SELECT DISTINCT ascl_uniqueid, ascl_id "
	"FROM _asteriskcalls "
	"WHERE ascl_direction != (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = '%1')"
	"ORDER BY ascl_uniqueid, ascl_id ASC "
	") "
	"SELECT uniq.ascl_id FROM uniq "
	")").arg(CONST_UID_ASTERISK_DIRECTION_TRANSFER));
}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::TransferFilter(const QString &UniqueID)
{
	GetQueryModel()->SetClassFilter("ascl.ascl_uniqueid = '" + UniqueID + "' "
									"AND ascl.ascl_direction = (SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = '" + CONST_UID_ASTERISK_DIRECTION_TRANSFER + "')");
}
//-----------------------------------------------------------------------------
ISUuid ISAsteriskCallsListForm::GetStatusCall()
{
	ISUuid StatusUID;
	ISQuery qSelect(QS_ASTERISK_CALL_STATUS);
	qSelect.BindValue(":ObjectID", GetObjectID());
	if (qSelect.ExecuteFirst())
	{
		StatusUID = qSelect.ReadColumn("asds_uid");
	}
	return StatusUID;
}
//-----------------------------------------------------------------------------
