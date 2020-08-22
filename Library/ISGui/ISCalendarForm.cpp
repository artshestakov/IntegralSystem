#include "ISCalendarForm.h"
#include "ISConstants.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISGui.h"
#include "ISCalendarObjectForm.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISCalendarEventForm.h"
#include "ISInputDialog.h"
#include "ISCore.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
static QString QS_CALENDAR = PREPARE_QUERY("SELECT cldr_id, cldr_date, cldr_timealert, cldr_name, cldr_text, cldr_closed "
										   "FROM _calendar "
										   "WHERE NOT cldr_isdeleted "
										   "AND cldr_user = currentuserid() "
										   "AND cldr_date = :Date "
										   "ORDER BY cldr_id DESC");
//-----------------------------------------------------------------------------
static QString QD_CALENDAR = PREPARE_QUERY("DELETE FROM _calendar WHERE cldr_id = :CalendarID");
//-----------------------------------------------------------------------------
static QString QS_CALENDAR_OVERDUE = PREPARE_QUERY("SELECT cldr_id "
												   "FROM _calendar "
												   "WHERE cldr_user = currentuserid() "
												   "AND NOT cldr_closed "
												   "AND cldr_date <= CURRENT_DATE "
												   "AND cldr_timealert <= CURRENT_TIME");
//-----------------------------------------------------------------------------
static QString QS_CALENDAR_SEARCH = PREPARE_QUERY("SELECT cldr_id, cldr_date, cldr_timealert, cldr_name, cldr_text, cldr_closed "
												  "FROM _calendar "
												  "WHERE NOT cldr_isdeleted "
												  "AND cldr_user = currentuserid() "
												  "ORDER BY cldr_id DESC");
//-----------------------------------------------------------------------------
static QString QS_CALENDAR_EVENT_DATE = PREPARE_QUERY("SELECT cldr_date FROM _calendar WHERE cldr_id = :CalendarID");
//-----------------------------------------------------------------------------
ISCalendarForm::ISCalendarForm(QWidget *parent)
	: ISParagraphBaseForm(parent),
	FirstUpdate(false)
{
	MainLayout = new QHBoxLayout();
	setLayout(MainLayout);

	QVBoxLayout *LayoutCentral = new QVBoxLayout();
	MainLayout->addLayout(LayoutCentral);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
	LayoutCentral->addWidget(ToolBar);

	QWidget *Spacer = new QWidget(ToolBar);
	Spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ToolBar->addWidget(Spacer);

	ActionCreate = ISControls::CreateActionCreate(ToolBar);
	ActionCreate->setText(LANG("CalendarForm.CreateEvent"));
	ActionCreate->setToolTip(LANG("CalendarForm.CreateEvent.ToolTip"));
	connect(ActionCreate, &QAction::triggered, this, &ISCalendarForm::Create);
	ToolBar->addAction(ActionCreate);

	ActionDateTo = new QAction(ToolBar);
	ActionDateTo->setText(LANG("CalendarForm.CalendarDateTo"));
	ActionDateTo->setToolTip(LANG("CalendarForm.CalendarDateTo"));
	ActionDateTo->setIcon(BUFFER_ICONS("CalendarMain.DateTo"));
	connect(ActionDateTo, &QAction::triggered, this, &ISCalendarForm::DateTo);
	ToolBar->addAction(ActionDateTo);

	ActionToday = new QAction(ToolBar);
	ActionToday->setText(LANG("Today"));
	ActionToday->setToolTip(LANG("Today"));
	ActionToday->setIcon(BUFFER_ICONS("CalendarMain.Today"));
	connect(ActionToday, &QAction::triggered, this, &ISCalendarForm::ToCurrentDate);
	ToolBar->addAction(ActionToday);

	ActionSettings = new QAction(ToolBar);
	ActionSettings->setText(LANG("Settings"));
	ActionSettings->setToolTip(LANG("Settings"));
	ActionSettings->setIcon(BUFFER_ICONS("Settings"));
	connect(ActionSettings, &QAction::triggered, this, &ISCalendarForm::ShowSettingsForm);
	ToolBar->addAction(ActionSettings);

	ActionEdit = ISControls::CreateActionEdit(this);
	ActionEdit->setEnabled(false);
	connect(ActionEdit, &QAction::triggered, this, &ISCalendarForm::Edit);

	ActionDelete = ISControls::CreateActionDelete(this);
	ActionDelete->setText(LANG("CalendarForm.Delete"));
	ActionDelete->setToolTip(LANG("CalendarForm.Delete"));
	ActionDelete->setEnabled(false);
	connect(ActionDelete, &QAction::triggered, this, &ISCalendarForm::Delete);

	ActionCloseEvent = new QAction(this);
	ActionCloseEvent->setText(LANG("CalendarCloseEvent"));
	ActionCloseEvent->setIcon(BUFFER_ICONS("Close"));
	ActionCloseEvent->setEnabled(false);
	connect(ActionCloseEvent, &QAction::triggered, this, &ISCalendarForm::CloseEvent);

	ActionDateEvent = new QAction(this);
	ActionDateEvent->setText(LANG("CalendarForm.OnDateEvent"));
	ActionDateEvent->setIcon(BUFFER_ICONS("Arrow.Left"));
	ActionDateEvent->setEnabled(false);
	connect(ActionDateEvent, &QAction::triggered, this, &ISCalendarForm::DateEvent);

	CalendarPanel = new ISCalendarPanel(this);
	connect(CalendarPanel, &ISCalendarPanel::selectionChanged, this, &ISCalendarForm::SelectedDateChanged);
	LayoutCentral->addWidget(CalendarPanel);

	QVBoxLayout *LayoutRight = new QVBoxLayout();
	QWidget *Panel = new QWidget(this);
	Panel->setLayout(LayoutRight);
	MainLayout->addWidget(Panel);

	QHBoxLayout *LayoutSelectedDate = new QHBoxLayout();
	LayoutSelectedDate->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutRight->addLayout(LayoutSelectedDate);

	LabelDayNumber = new QLabel(Panel);
	LabelDayNumber->setFont(ISDefines::Gui::FONT_TAHOMA_35);
	LayoutSelectedDate->addWidget(LabelDayNumber, 0, Qt::AlignVCenter);

	QVBoxLayout *LayoutSelectedDateRight = new QVBoxLayout();
	LayoutSelectedDate->addLayout(LayoutSelectedDateRight);

	LabelDayName = new QLabel(this);
	LabelDayName->setFont(ISDefines::Gui::FONT_TAHOMA_18);
	LayoutSelectedDateRight->addWidget(LabelDayName);

	LabelMonthYear = new QLabel(this);
	LabelMonthYear->setFont(ISDefines::Gui::FONT_TAHOMA_12);
	LayoutSelectedDateRight->addWidget(LabelMonthYear);

	LayoutSelectedDate->addStretch();

	EditSearch = new ISLineEdit(this);
	EditSearch->SetPlaceholderText(LANG("CalendarForm.EnteringSearchQuery"));
	connect(EditSearch, &ISLineEdit::ValueChange, this, &ISCalendarForm::SearchChanged);
	LayoutRight->addWidget(EditSearch);

	GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LANG("CalendarForm.Events"));
	GroupBox->setLayout(new QVBoxLayout());
	LayoutRight->addWidget(GroupBox);

	ListWidget = new ISCalendarEventsWidget(GroupBox);
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISCalendarForm::ItemDoubleClicked);
	connect(ListWidget, &QListWidget::itemSelectionChanged, this, &ISCalendarForm::ItemSelectionChanged);
	GroupBox->layout()->addWidget(ListWidget);

	ListWidget->addAction(ActionEdit);
	ListWidget->addAction(ActionDelete);
	ListWidget->addAction(ActionCloseEvent);
	ListWidget->addAction(ActionDateEvent);

	QAction *ActionSearch = new QAction(this);
	ActionSearch->setShortcut(Qt::Key_F8);
	connect(ActionSearch, &QAction::triggered, EditSearch, &ISLineEdit::SetFocus);
	addAction(ActionSearch);

	QTimer::singleShot(5000, this, &ISCalendarForm::ShowOverdueEvents);
}
//-----------------------------------------------------------------------------
ISCalendarForm::~ISCalendarForm()
{

}
//-----------------------------------------------------------------------------
void ISCalendarForm::Invoke()
{
	ISParagraphBaseForm::Invoke();
	CalendarPanel->selectionChanged();

	if (!FirstUpdate)
	{
		CalendarPanel->UpdateCells();
		FirstUpdate = true;
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::ShowOverdueEvents()
{
	ISQuery qSelect(QS_CALENDAR_OVERDUE);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ShowEventForm(qSelect.ReadColumn("cldr_id").toInt());
		}
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::SelectedDateChanged()
{
	ISGui::SetWaitGlobalCursor(true);

	QDate Date = CalendarPanel->selectedDate();
	LabelDayNumber->setText(QString::number(Date.day()));
	LabelDayName->setText(Date == QDate::currentDate() ? QString("%1 (%2)").arg(Date.longDayName(Date.dayOfWeek())).arg(LANG("Today")) : Date.longDayName(Date.dayOfWeek()));
	LabelMonthYear->setText(QString("%1 %2").arg(Date.longMonthName(Date.month())).arg(Date.year()));
	EditSearch->Clear();
	ListWidget->Clear();

	ISQuery qSelect(QS_CALENDAR);
	qSelect.BindValue(":Date", Date);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int ID = qSelect.ReadColumn("cldr_id").toInt();
			QTime TimeAlert = qSelect.ReadColumn("cldr_timealert").toTime();
			QString Name = qSelect.ReadColumn("cldr_name").toString();
			QString Text = qSelect.ReadColumn("cldr_text").toString();
			bool Closed = qSelect.ReadColumn("cldr_closed").toBool();

			AddEventItem(ID, Name, Text, TimeAlert, Closed);
		}
	}

	GroupBox->setTitle(LANG("CalendarForm.Events"));
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISCalendarForm::Create()
{
	ISCalendarObjectForm *CalendarObjectForm = dynamic_cast<ISCalendarObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_New, "_Calendar"));
	CalendarObjectForm->SetDate(CalendarPanel->selectedDate());
	connect(CalendarObjectForm, &ISCalendarObjectForm::UpdateList, this, &ISCalendarForm::SelectedDateChanged);
	connect(CalendarObjectForm, &ISCalendarObjectForm::UpdateList, CalendarPanel, &ISCalendarPanel::UpdateCells);
	ISGui::ShowObjectForm(CalendarObjectForm);
}
//-----------------------------------------------------------------------------
void ISCalendarForm::DateTo()
{
	QDate Date = ISInputDialog::GetDate(LANG("CalendarForm.CalendarDateTo"), LANG("SelectDate") + ':');
	if (Date.isValid())
	{
		CalendarPanel->setSelectedDate(Date);
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::ToCurrentDate()
{
	CalendarPanel->Today();
}
//-----------------------------------------------------------------------------
void ISCalendarForm::ShowSettingsForm()
{
	ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_CALENDAR);
}
//-----------------------------------------------------------------------------
void ISCalendarForm::Edit()
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidget->currentItem()));
	if (EventItem)
	{
		if (!EventItem->GetClosed())
		{
			EditEvent(EventItem->GetCalendarID());
		}
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::Delete()
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidget->currentItem()));
	if (EventItem)
	{
		DeleteEvent(EventItem->GetCalendarID());
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::CloseEvent()
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidget->currentItem()));
	if (EventItem)
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CalendarCloseEvent")))
		{
			if (ISCore::CalendarCloseEvent(EventItem->GetCalendarID()))
			{
				CalendarPanel->selectionChanged();
				CalendarPanel->UpdateCells();
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::DateEvent()
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidget->currentItem()));
	if (EventItem)
	{
		ISQuery qSelect(QS_CALENDAR_EVENT_DATE);
		qSelect.BindValue(":CalendarID", EventItem->GetCalendarID());
		if (qSelect.ExecuteFirst())
		{
			QDate Date = qSelect.ReadColumn("cldr_date").toDate();
			CalendarPanel->setSelectedDate(Date);
		}
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidgetItem));
	if (!EventItem->GetClosed())
	{
		EditEvent(EventItem->GetCalendarID());
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::ItemSelectionChanged()
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidget->currentItem()));
	if (EventItem)
	{
		ActionEdit->setEnabled(!EventItem->GetClosed());
		ActionDelete->setEnabled(!EventItem->GetClosed());
		ActionCloseEvent->setEnabled(!EventItem->GetClosed());
		ActionDateEvent->setEnabled(true);
	}
	else
	{
		ActionEdit->setEnabled(false);
		ActionDelete->setEnabled(false);
		ActionCloseEvent->setEnabled(false);
		ActionDateEvent->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::EditEvent(int CalendarID)
{
	ISCalendarObjectForm *CalendarObjectForm = dynamic_cast<ISCalendarObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_Calendar", CalendarID));
	connect(CalendarObjectForm, &ISCalendarObjectForm::UpdateList, this, &ISCalendarForm::SelectedDateChanged);
	connect(CalendarObjectForm, &ISCalendarObjectForm::UpdateList, CalendarPanel, &ISCalendarPanel::UpdateCells);
	ISGui::ShowObjectForm(CalendarObjectForm);
}
//-----------------------------------------------------------------------------
void ISCalendarForm::DeleteEvent(int CalendarID)
{
	if (ISMessageBox::ShowQuestion(CalendarPanel, LANG("Message.Question.DeleteCalendarEvent")))
	{
		ISQuery qDelete(QD_CALENDAR);
		qDelete.BindValue(":CalendarID", CalendarID);
		if (qDelete.Execute())
		{
			CalendarPanel->selectionChanged();
			CalendarPanel->UpdateCells();
		}
	}
}
//-----------------------------------------------------------------------------
void ISCalendarForm::SearchChanged(const QVariant &value)
{
	ListWidget->Clear();

	if (value.isValid())
	{
		GroupBox->setTitle(LANG("CalendarForm.SearchResults"));
		QString SearchValue = value.toString();

		ISQuery qSelect(QS_CALENDAR_SEARCH);
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				int ID = qSelect.ReadColumn("cldr_id").toInt();
				QDate Date = qSelect.ReadColumn("cldr_date").toDate();
				QTime TimeAlert = qSelect.ReadColumn("cldr_timealert").toTime();
				QString Name = qSelect.ReadColumn("cldr_name").toString().toLower();
				QString Text = qSelect.ReadColumn("cldr_text").toString().toLower();
				bool Closed = qSelect.ReadColumn("cldr_closed").toBool();

				if (Name.contains(SearchValue) || Text.contains(SearchValue))
				{
					AddEventItem(ID, Name, Text, TimeAlert, Closed)->SetVisibleDate(Date);
				}
			}
		}
	}
	else
	{
		CalendarPanel->selectionChanged();
	}
}
//-----------------------------------------------------------------------------
ISCalendarEventItem* ISCalendarForm::AddEventItem(int CalendarID, const QString &Name, const QString &Text, const QTime &Time, bool Closed)
{
	ISCalendarEventItem *EventItem = new ISCalendarEventItem(CalendarID, Name, Text, Time, Closed, ListWidget);
	QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
	ListWidgetItem->setSizeHint(EventItem->sizeHint());
	ListWidget->setItemWidget(ListWidgetItem, EventItem);
	connect(EventItem, &ISCalendarEventItem::SizeHintChanged, [=] { ListWidgetItem->setSizeHint(EventItem->sizeHint()); });
	return EventItem;
}
//-----------------------------------------------------------------------------
void ISCalendarForm::CalendarEvent(const QVariantMap &VariantMap)
{
	ShowEventForm(VariantMap.value("Payload").toInt());
}
//-----------------------------------------------------------------------------
void ISCalendarForm::ShowEventForm(int CalendarID)
{
	ISCalendarEventForm *CalendarEventForm = new ISCalendarEventForm(CalendarID);
	CalendarEventForm->show();
	CalendarEventForm->activateWindow();
	connect(CalendarEventForm, &ISCalendarEventForm::ClosedEvent, this, &ISCalendarForm::Invoke);
}
//-----------------------------------------------------------------------------
