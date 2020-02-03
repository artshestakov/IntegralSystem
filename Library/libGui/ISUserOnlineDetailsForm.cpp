#include "ISUserOnlineDetailsForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISButtonClose.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_PG_STAT_ACTIVITY = PREPARE_QUERY("SELECT "
												   "userfullname(:UserID), "
												   "userlogin(:UserID), "
												   "pid, "
												   "split_part(application_name, ' ', 2), "
												   "client_addr, "
												   "client_hostname, "
												   "client_port, "
												   "backend_start, "
												   "(now() - backend_start)::TIME, "
												   "state "
												   "FROM pg_stat_activity "
												   "WHERE usename = userlogin(:UserID)");
//-----------------------------------------------------------------------------
ISUserOnlineDetailsForm::ISUserOnlineDetailsForm(int user_id, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("MonitorActivity.DetailsSession"));
	setWindowIcon(BUFFER_ICONS("Information"));
	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_10_PX);

	FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	ISQuery qSelect(QS_PG_STAT_ACTIVITY);
	qSelect.BindValue(":UserID", user_id);
	qSelect.ExecuteFirst();

	AddLabel(LANG("MonitorActivity.UserOnlineDetails.User"), qSelect.ReadColumn("userfullname").toString());
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.Login"), qSelect.ReadColumn("userlogin").toString());
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.PID"), qSelect.ReadColumn("pid").toString());
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.ClientVersion"), qSelect.ReadColumn("split_part").toString());
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.ClientAddress"), qSelect.ReadColumn("client_addr").toString());
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.HostName"), qSelect.ReadColumn("client_hostname").toString());
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.Port"), qSelect.ReadColumn("client_port").toString());
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.BackendStart"), ISGui::ConvertDateTimeToString(qSelect.ReadColumn("backend_start").toDateTime(), DATE_FORMAT_V2, TIME_FORMAT_V1));
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.Time"), qSelect.ReadColumn("time").toString());
	AddLabel(LANG("MonitorActivity.UserOnlineDetails.State"), GetState(qSelect.ReadColumn("state").toString()));

	GetMainLayout()->addStretch();
	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISUserOnlineDetailsForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISUserOnlineDetailsForm::~ISUserOnlineDetailsForm()
{

}
//-----------------------------------------------------------------------------
void ISUserOnlineDetailsForm::AddLabel(const QString &LabelName, const QString &LabelValue)
{
	QLabel *Label = new QLabel(this);
	Label->setText(LabelValue);
	ISGui::SetFontWidgetBold(Label, true);
	FormLayout->addRow(LabelName + ':', Label);
}
//-----------------------------------------------------------------------------
QString ISUserOnlineDetailsForm::GetState(const QString &State) const
{
	QString Result = SYMBOL_MINUS;
	if (State == "active")
	{
		Result = LANG("MonitorActivity.UserOnlineDetails.State.Active");
	}
	else if (State == "idle")
	{
		Result = LANG("MonitorActivity.UserOnlineDetails.State.Idle");
	}

	return Result;
}
//-----------------------------------------------------------------------------
