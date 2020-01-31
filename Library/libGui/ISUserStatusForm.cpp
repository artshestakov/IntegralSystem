#include "StdAfx.h"
#include "ISUserStatusForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISControls.h"
#include "ISGui.h"
#include "ISButtonClose.h"
#include "ISProtocol.h"
//-----------------------------------------------------------------------------
static QString QS_USER_STATUS = PREPARE_QUERY("SELECT usrs_currentstatus, usts_name "
											  "FROM _users "
											  "LEFT JOIN _userstatus ON usts_id = usrs_currentstatus "
											  "WHERE usrs_id = currentuserid()");
//-----------------------------------------------------------------------------
static QString QS_USER_STATUSES = PREPARE_QUERY("SELECT usts_uid, usts_name, usts_icon "
												"FROM _userstatus "
												"WHERE NOT usts_isdeleted "
												"ORDER BY usts_order");
//-----------------------------------------------------------------------------
static QString QU_USER_STATUS = PREPARE_QUERY("UPDATE _users SET "
											  "usrs_currentstatus = (SELECT usts_id FROM _userstatus WHERE usts_uid = :StatusUID) "
											  "WHERE usrs_id = currentuserid()");
//-----------------------------------------------------------------------------
static QString QU_USER_STATUS_CLEAR = PREPARE_QUERY("UPDATE _users SET "
													"usrs_currentstatus = NULL "
													"WHERE usrs_id = currentuserid()");
//-----------------------------------------------------------------------------
ISUserStatusForm::ISUserStatusForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("StatusUser"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_5_PX);

	QLabel *LabelCurrentStatus = new QLabel(this);
	LabelCurrentStatus->setFont(FONT_TAHOMA_10_BOLD);
	LabelCurrentStatus->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	GetMainLayout()->addWidget(LabelCurrentStatus);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	ISQuery qSelectCurrentStatus(QS_USER_STATUS);
	if (qSelectCurrentStatus.ExecuteFirst())
	{
		int StatusID = qSelectCurrentStatus.ReadColumn("usrs_currentstatus").toInt();
		if (StatusID)
		{
			QString StatusName = qSelectCurrentStatus.ReadColumn("usts_name").toString();
			LabelCurrentStatus->setText(LANG("StatusUser") + ": " + StatusName);
		}
		else
		{
			LabelCurrentStatus->setText(LANG("StatusUser") + ": " + LANG("StatusUser.NotInstalled"));
		}
	}

	ISQuery qSelectStatuses(QS_USER_STATUSES);
	if (qSelectStatuses.Execute())
	{
		while (qSelectStatuses.Next())
		{
			ISUuid StatusUID = qSelectStatuses.ReadColumn("usts_uid");
			QString StatusName = qSelectStatuses.ReadColumn("usts_name").toString();
			QIcon StatusIcon = BUFFER_ICONS(qSelectStatuses.ReadColumn("usts_icon").toString());

			QCommandLinkButton *CommandLinkButton = new QCommandLinkButton(this);
			CommandLinkButton->setText(StatusName);
			CommandLinkButton->setIcon(StatusIcon);
			CommandLinkButton->setObjectName(StatusUID);
			CommandLinkButton->setCursor(CURSOR_POINTING_HAND);
			connect(CommandLinkButton, &QCommandLinkButton::clicked, this, &ISUserStatusForm::StatusChange);
			GetMainLayout()->addWidget(CommandLinkButton);
		}

		GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

		QCommandLinkButton *CommandLinkButtonClear = new QCommandLinkButton(this);
		CommandLinkButtonClear->setText(LANG("ClearStatus"));
		CommandLinkButtonClear->setCursor(CURSOR_POINTING_HAND);
		connect(CommandLinkButtonClear, &QCommandLinkButton::clicked, this, &ISUserStatusForm::StatusClear);
		GetMainLayout()->addWidget(CommandLinkButtonClear);
	}

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISUserStatusForm::Close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISUserStatusForm::~ISUserStatusForm()
{

}
//-----------------------------------------------------------------------------
void ISUserStatusForm::StatusChange()
{
	ISGui::SetWaitGlobalCursor(true);

	QCommandLinkButton *SenderButton = dynamic_cast<QCommandLinkButton*>(sender());

	ISQuery qUpdateStatus(QU_USER_STATUS);
	qUpdateStatus.BindValue(":StatusUID", SenderButton->objectName());
	if (qUpdateStatus.Execute())
	{
		ISProtocol::Insert(true, CONST_UID_PROTOCOL_USER_STATUS_CHANGE, QString(), QString(), QVariant(), SenderButton->text());
		SetResult(true);
		close();
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserStatusForm::StatusClear()
{
	ISQuery qStatusClear(QU_USER_STATUS_CLEAR);
	if (qStatusClear.Execute())
	{
		SetResult(true);
		close();
	}
}
//-----------------------------------------------------------------------------
void ISUserStatusForm::Close()
{
	SetResult(false);
	close();
}
//-----------------------------------------------------------------------------
