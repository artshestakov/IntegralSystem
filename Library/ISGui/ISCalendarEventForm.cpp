#include "ISCalendarEventForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISButtons.h"
#include "ISButtons.h"
#include "ISSettings.h"
#include "ISConstants.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISAssert.h"
#include "ISCore.h"
//-----------------------------------------------------------------------------
static QString QS_CALENDAR = PREPARE_QUERY("SELECT cldr_date, cldr_timealert, cldr_name, cldr_text "
										   "FROM _calendar "
										   "WHERE cldr_id = :CalendarID");
//-----------------------------------------------------------------------------
static QString QU_DEFER = PREPARE_QUERY("UPDATE _calendar SET "
										"cldr_date = :Date, "
										"cldr_timealert = :Time "
										"WHERE cldr_id = :CalendarID");
//-----------------------------------------------------------------------------
ISCalendarEventForm::ISCalendarEventForm(int calendar_id, QWidget *parent)
	: ISInterfaceForm(parent),
	CalendarID(calendar_id),
	CloseEvent(true),
	Sound(nullptr)
{
	ISQuery qSelectEvent(QS_CALENDAR);
	qSelectEvent.BindValue(":CalendarID", calendar_id);
	IS_ASSERT(qSelectEvent.ExecuteFirst(), qSelectEvent.GetErrorString());

	QDate EventDate = qSelectEvent.ReadColumn("cldr_date").toDate();
	QTime EventTimeAlert = qSelectEvent.ReadColumn("cldr_timealert").toTime();
	QString EventName = qSelectEvent.ReadColumn("cldr_name").toString();
	QString EventText = qSelectEvent.ReadColumn("cldr_text").toString();

	setWindowTitle(LANG("Reminder"));
	setWindowIcon(BUFFER_ICONS("Bell"));
	setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);

	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *LayoutInfo = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutInfo, 0);

	QLabel *LabelImage = new QLabel(this);
	LabelImage->setPixmap(BUFFER_ICONS("Calendar").pixmap(ISDefines::Gui::SIZE_32_32));
	LayoutInfo->addWidget(LabelImage);

	QVBoxLayout *Layout = new QVBoxLayout();
	LayoutInfo->addLayout(Layout);

	QLabel *LabelName = new QLabel(this);
	LabelName->setText(EventName);
	LabelName->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LabelName->setWordWrap(true);
	LabelName->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
	Layout->addWidget(LabelName);

	QHBoxLayout *LayoutDate = new QHBoxLayout();
	Layout->addLayout(LayoutDate);

	QLabel *LabelOverdueIcon = new QLabel(this);
	LabelOverdueIcon->setVisible(false);
	LayoutDate->addWidget(LabelOverdueIcon);

	QLabel *LabelDate = new QLabel(this);
	LabelDate->setText(QString("%1: %2 %3 %4 %5").arg(LANG("Date")).arg(EventDate.day()).arg(QDate::longMonthName(EventDate.month())).arg(EventDate.year()).arg(LANG("Year.Reduction")));
	LayoutDate->addWidget(LabelDate);

	if (EventDate == QDate::currentDate()) //Событие сегодня
	{
		LabelDate->setText(LabelDate->text() + " (" + LANG("Today") + ')');
	}
	else if (EventDate < QDate::currentDate()) //Событие просрочено
	{
		LabelOverdueIcon->setPixmap(BUFFER_ICONS("Importance.High").pixmap(ISDefines::Gui::SIZE_16_16));
		LabelOverdueIcon->setVisible(true);

		LabelDate->setText(LabelDate->text() + " (" + LANG("OverdueEvent") + ')');
		LabelDate->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Red"));
		LabelDate->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	}

	QLabel *LabelTime = new QLabel(this);
	LabelTime->setText(LANG("Time") + ": " + EventTimeAlert.toString(FORMAT_TIME_V1));
	Layout->addWidget(LabelTime);

	LayoutInfo->addStretch();

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LANG("Text"));
	GroupBox->setLayout(new QVBoxLayout());
	GetMainLayout()->addWidget(GroupBox);

	ISTextEdit *TextEdit = new ISTextEdit(GroupBox);
	TextEdit->SetValue(EventText);
	TextEdit->SetReadOnly(true);
	GroupBox->layout()->addWidget(TextEdit);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutBottom);

	ComboEdit = new ISComboTimeEdit(this);
	LayoutBottom->addWidget(ComboEdit);

	ISPushButton *ButtonDefer = new ISPushButton(this);
	ButtonDefer->setText(LANG("Defer"));
	connect(ButtonDefer, &ISPushButton::clicked, this, &ISCalendarEventForm::Defer);
	LayoutBottom->addWidget(ButtonDefer);

		LayoutBottom->addStretch();

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("CalendarCloseEvent"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISCalendarEventForm::EventClose);
	LayoutBottom->addWidget(ButtonClose);

	TimerSound = new QTimer(this);
	TimerSound->setInterval(CALENDAR_AUDIO_TIMEOUT);

	if (SETTING_BOOL(CONST_UID_SETTING_SOUND_CALENDAREVENT))
	{
		connect(TimerSound, &QTimer::timeout, [=]
		{
			Sound->play();
		});
		
		Sound = new QSound(BUFFER_AUDIO("CalendarEvent"), this);
		Sound->play();
	}

	TimerSound->start();
}
//-----------------------------------------------------------------------------
ISCalendarEventForm::~ISCalendarEventForm()
{

}
//-----------------------------------------------------------------------------
void ISCalendarEventForm::AfterShowEvent()
{
	ISInterfaceForm::AfterShowEvent();
	FlashingStart(1000, ISDefines::Gui::COLOR_CALENDAR_EVENT_FORM_FLASH);
}
//-----------------------------------------------------------------------------
void ISCalendarEventForm::closeEvent(QCloseEvent *e)
{
	if (CloseEvent)
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CalendarEventFormClose")))
		{
			EventClose();
		}
		else
		{
			e->ignore();
			return;
		}
	}

	ISInterfaceForm::closeEvent(e);
}
//-----------------------------------------------------------------------------
void ISCalendarEventForm::Defer()
{
	ISGui::SetWaitGlobalCursor(true);

	int Minute = ComboEdit->GetValue().toInt();
	QDateTime DateTime = QDateTime::currentDateTime().addSecs(Minute * 60);
	
	ISQuery qUpdate(QU_DEFER);
	qUpdate.BindValue(":Date", DateTime.date());
	qUpdate.BindValue(":Time", DateTime.time());
	qUpdate.BindValue(":CalendarID", CalendarID);
	if (qUpdate.Execute())
	{
		CloseEvent = false;
		emit ClosedEvent();
		close();
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISCalendarEventForm::EventClose()
{
	ISGui::SetWaitGlobalCursor(true);

	if (ISCore::CalendarCloseEvent(CalendarID))
	{
		CloseEvent = false;
		emit ClosedEvent();
		close();
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
