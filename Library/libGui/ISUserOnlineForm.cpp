#include "StdAfx.h"
#include "ISUserOnlineForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
static QString QS_USER_ONLINE = PREPARE_QUERY("SELECT uotp_uid, uotp_name, uonl_time "
											  "FROM _useronline "
											  "LEFT JOIN _useronlinetype ON uotp_id = uonl_type "
											  "WHERE uonl_user = :UserID "
											  "AND uonl_date = :Date "
											  "ORDER BY uonl_creationdate");
//-----------------------------------------------------------------------------
ISUserOnlineForm::ISUserOnlineForm(int user_id, const QString &user_name, QWidget *parent) : ISInterfaceForm(parent)
{
	UserID = user_id;

	setWindowTitle(LOCALIZATION("MonitorActivity.ActivityUser") + ": " + user_name);
	setWindowIcon(BUFFER_ICONS("User"));
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	LayoutTitle->addWidget(new QLabel(LOCALIZATION("Date") + ":", this));

	DateEdit = new ISDateEdit(this);
	DateEdit->SetCheckEnable(Qt::Checked);
	DateEdit->SetVisibleCheck(false);
	connect(DateEdit, &ISDateEdit::DataChanged, this, &ISUserOnlineForm::Update);
	LayoutTitle->addWidget(DateEdit);

	ISPushButton *ButtonUpdate = new ISPushButton(this);
	ButtonUpdate->setText(LOCALIZATION("Update"));
	ButtonUpdate->setIcon(BUFFER_ICONS("Update"));
	connect(ButtonUpdate, &ISPushButton::clicked, this, &ISUserOnlineForm::Update);
	LayoutTitle->addWidget(ButtonUpdate);

	LabelCountTime = new QLabel(this);
	LabelCountTime->setVisible(false);
	ISGui::SetFontWidgetBold(LabelCountTime, true);
	LayoutTitle->addWidget(LabelCountTime);

	LabelNotRegistered = new QLabel(this);
	LabelNotRegistered->setVisible(false);
	LabelNotRegistered->setText(LOCALIZATION("ActivityUserNotRegisteredFromDate").arg(DateEdit->GetValue().toDate().toString(DATE_FORMAT_V2)));
	ISGui::SetFontWidgetBold(LabelNotRegistered, true);
	LayoutTitle->addWidget(LabelNotRegistered);

	LayoutTitle->addStretch();
	
	ListWidget = new ISListWidget(this);
	ListWidget->setAlternatingRowColors(true);
	GetMainLayout()->addWidget(ListWidget);

	Update();
}
//-----------------------------------------------------------------------------
ISUserOnlineForm::~ISUserOnlineForm()
{

}
//-----------------------------------------------------------------------------
void ISUserOnlineForm::Update()
{
	ISGui::SetWaitGlobalCursor(true);

	while (ListWidget->count())
	{
		ListWidget->RemoveBeginItem();
	}

	ISQuery qSelect(QS_USER_ONLINE);
	qSelect.BindValue(":UserID", UserID);
	qSelect.BindValue(":Date", DateEdit->GetValue());
	if (qSelect.Execute())
	{
		if (qSelect.GetCountResultRows())
		{
			LabelCountTime->setVisible(true);
			LabelNotRegistered->setVisible(false);

			int SecondsActive = 0;
			int SecondsInactive = 0;
			int InactiveTimeout = ISDatabase::GetInstance().GetValue("_Users", "InactiveTimeout", UserID).toInt() * 60;
			QTime PreviousTime(0, 0);

			while (qSelect.Next())
			{
				ISUuid TypeUID = qSelect.ReadColumn("uotp_uid");
				QTime Time = qSelect.ReadColumn("uonl_time").toTime();
				QString OnlineType = qSelect.ReadColumn("uotp_name").toString();

				QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
				ListWidgetItem->setText(Time.toString(TIME_FORMAT_V3) + ": " + OnlineType);
				ListWidgetItem->setData(Qt::UserRole, Time);
				ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));

				if (TypeUID == CONST_UID_USER_ONLINE_TYPE_ACTIVE)
				{
					SecondsActive += PreviousTime.secsTo(Time);
				}
				else if (TypeUID == CONST_UID_USER_ONLINE_TYPE_INACTIVE)
				{
					SecondsInactive += PreviousTime.secsTo(Time);
					SecondsInactive -= InactiveTimeout;
				}

				PreviousTime = Time;
			}

			QTime TimeActive = QTime(0, 0).addSecs(SecondsActive);
			QTime TimeInactive = QTime(0, 0).addSecs(SecondsInactive);
			LabelCountTime->setText(LOCALIZATION("MonitorActivity.ActivityUserCount").arg(TimeActive.toString(TIME_FORMAT_V3)).arg(TimeInactive.toString(TIME_FORMAT_V3)));
		}
		else
		{
			LabelCountTime->setVisible(false);
			LabelNotRegistered->setVisible(true);
		}
	}
	
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserOnlineForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
