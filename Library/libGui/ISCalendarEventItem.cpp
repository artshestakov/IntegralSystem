#include "ISCalendarEventItem.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISGui.h"
#include "ISSettings.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISCalendarEventItem::ISCalendarEventItem(int calendar_id, const QString &Name, const QString &Text, const QTime &Time, bool closed, QWidget *parent) : QWidget(parent)
{
	CalendarID = calendar_id;
	Closed = closed;

	setCursor(CURSOR_POINTING_HAND);

	QHBoxLayout *MainLayout = new QHBoxLayout();
	setLayout(MainLayout);

	QLabel *LabelImage = new QLabel(this);
	LabelImage->setPixmap(BUFFER_ICONS("CalendarMain.Indicator").pixmap(ISDefines::Gui::SIZE_32_32));
	MainLayout->addWidget(LabelImage, 0, Qt::AlignTop);

	QVBoxLayout *Layout = new QVBoxLayout();
	MainLayout->addLayout(Layout);

	QLabel *LabelName = new QLabel(this);
	LabelName->setWordWrap(true);
	LabelName->setText(Name);
	Layout->addWidget(LabelName);

	LabelDate = new QLabel(this);
	LabelDate->setVisible(false);
	LabelDate->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	Layout->addWidget(LabelDate);

	QLabel *LabelTime = new QLabel(this);
	LabelTime->setText(Time.toString(TIME_FORMAT_V1));
	LabelTime->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LabelTime->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	Layout->addWidget(LabelTime);

	MainLayout->addStretch();

	setEnabled(!closed);

	if (Text.length())
	{
		QString NoteViewType = SETTING_STRING(CONST_UID_SETTING_CALENDAR_NOTE_VIEW_TYPE);
		if (NoteViewType == "ToolTip")
		{
			setCursor(CURSOR_WHATS_THIS);
			setToolTip(ISGui::PrepareLongToolTip(Text, 450));
		}
		else if (NoteViewType == "List")
		{
			QLabel *LabelNote = new QLabel(this);
			LabelNote->setText(LANG("Note") + ":\n" + Text);
			LabelNote->setWordWrap(true);
			Layout->addWidget(LabelNote);
		}
	}
}
//-----------------------------------------------------------------------------
ISCalendarEventItem::~ISCalendarEventItem()
{

}
//-----------------------------------------------------------------------------
int ISCalendarEventItem::GetCalendarID() const
{
	return CalendarID;
}
//-----------------------------------------------------------------------------
bool ISCalendarEventItem::GetClosed() const
{
	return Closed;
}
//-----------------------------------------------------------------------------
void ISCalendarEventItem::SetVisibleDate(const QDate &Date)
{
	LabelDate->setText(Date.toString(DATE_FORMAT_V1));
	LabelDate->setVisible(true);

	adjustSize();
	emit SizeHintChanged();
}
//-----------------------------------------------------------------------------
