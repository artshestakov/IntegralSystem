#include "ISCalendarParagraph.h"
#include "ISConstants.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISGui.h"
#include "ISCalendarObjectForm.h"
#include "ISTcpQuery.h"
#include "ISDialogsCommon.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISCalendarParagraph::ISCalendarParagraph(QWidget *parent)
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

	QAction *ActionCreate = ISControls::CreateActionCreate(ToolBar);
	ActionCreate->setText(LANG("CalendarForm.CreateEvent"));
	ActionCreate->setToolTip(LANG("CalendarForm.CreateEvent.ToolTip"));
	connect(ActionCreate, &QAction::triggered, this, &ISCalendarParagraph::Create);
	ToolBar->addAction(ActionCreate);

	QAction *ActionDateTo = new QAction(ToolBar);
	ActionDateTo->setText(LANG("CalendarForm.CalendarDateTo"));
	ActionDateTo->setToolTip(LANG("CalendarForm.CalendarDateTo"));
	ActionDateTo->setIcon(BUFFER_ICONS("CalendarMain.DateTo"));
	connect(ActionDateTo, &QAction::triggered, this, &ISCalendarParagraph::DateTo);
	ToolBar->addAction(ActionDateTo);

	QAction *ActionToday = new QAction(ToolBar);
	ActionToday->setText(LANG("Today"));
	ActionToday->setToolTip(LANG("Today"));
	ActionToday->setIcon(BUFFER_ICONS("CalendarMain.Today"));
	connect(ActionToday, &QAction::triggered, this, &ISCalendarParagraph::ToCurrentDate);
	ToolBar->addAction(ActionToday);

	QAction *ActionSettings = new QAction(ToolBar);
	ActionSettings->setText(LANG("Settings"));
	ActionSettings->setToolTip(LANG("Settings"));
	ActionSettings->setIcon(BUFFER_ICONS("Settings"));
	connect(ActionSettings, &QAction::triggered, this, &ISCalendarParagraph::ShowSettingsForm);
	ToolBar->addAction(ActionSettings);

	ActionEdit = ISControls::CreateActionEdit(this);
	ActionEdit->setEnabled(false);
	connect(ActionEdit, &QAction::triggered, this, &ISCalendarParagraph::Edit);

	ActionDelete = ISControls::CreateActionDelete(this);
	ActionDelete->setText(LANG("CalendarForm.Delete"));
	ActionDelete->setToolTip(LANG("CalendarForm.Delete"));
	ActionDelete->setEnabled(false);
	connect(ActionDelete, &QAction::triggered, this, &ISCalendarParagraph::Delete);

	ActionCloseEvent = new QAction(this);
	ActionCloseEvent->setText(LANG("CalendarCloseEvent"));
	ActionCloseEvent->setIcon(BUFFER_ICONS("Close"));
	ActionCloseEvent->setEnabled(false);
	connect(ActionCloseEvent, &QAction::triggered, this, &ISCalendarParagraph::CloseEvent);

	CalendarPanel = new ISCalendarPanel(this);
	connect(CalendarPanel, &ISCalendarPanel::selectionChanged, this, &ISCalendarParagraph::SelectedDateChanged);
	connect(CalendarPanel, &ISCalendarPanel::currentPageChanged, this, static_cast<void(ISCalendarParagraph::*)(int, int)>(&ISCalendarParagraph::ReloadEvents));
	LayoutCentral->addWidget(CalendarPanel);

	QVBoxLayout *LayoutRight = new QVBoxLayout();
	QWidget *Panel = new QWidget(this);
	Panel->setLayout(LayoutRight);
	MainLayout->addWidget(Panel);

	QHBoxLayout *LayoutSelectedDate = new QHBoxLayout();
	LayoutSelectedDate->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutRight->addLayout(LayoutSelectedDate);

	QDate CurrentDate = QDate::currentDate();

	LabelDayNumber = new QLabel(QString::number(CurrentDate.day()), Panel);
	LabelDayNumber->setFont(ISDefines::Gui::FONT_TAHOMA_35);
	LayoutSelectedDate->addWidget(LabelDayNumber, 0, Qt::AlignVCenter);

	QVBoxLayout *LayoutSelectedDateRight = new QVBoxLayout();
	LayoutSelectedDate->addLayout(LayoutSelectedDateRight);

	LabelDayName = new QLabel(CurrentDate == QDate::currentDate() ?
		QString("%1 (%2)").arg(CurrentDate.longDayName(CurrentDate.dayOfWeek())).arg(LANG("Today")) :
		CurrentDate.longDayName(CurrentDate.dayOfWeek()), this);
	LabelDayName->setFont(ISDefines::Gui::FONT_TAHOMA_18);
	LayoutSelectedDateRight->addWidget(LabelDayName);

	LabelMonthYear = new QLabel(QString("%1 %2").arg(CurrentDate.longMonthName(CurrentDate.month())).arg(CurrentDate.year()), this);
	LabelMonthYear->setFont(ISDefines::Gui::FONT_TAHOMA_12);
	LayoutSelectedDateRight->addWidget(LabelMonthYear);

	LayoutSelectedDate->addStretch();

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LANG("CalendarForm.Events"));
	GroupBox->setLayout(new QVBoxLayout());
	LayoutRight->addWidget(GroupBox);

	ListWidget = new ISCalendarEventsWidget(GroupBox);
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISCalendarParagraph::ItemDoubleClicked);
	connect(ListWidget, &QListWidget::itemSelectionChanged, this, &ISCalendarParagraph::ItemSelectionChanged);
	GroupBox->layout()->addWidget(ListWidget);

	ListWidget->addAction(ActionEdit);
	ListWidget->addAction(ActionDelete);
	ListWidget->addAction(ActionCloseEvent);
}
//-----------------------------------------------------------------------------
ISCalendarParagraph::~ISCalendarParagraph()
{

}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::Invoke()
{
	ISParagraphBaseForm::Invoke();
	if (!FirstUpdate)
	{
		QDate CurrentDate = QDate::currentDate();
		ReloadEvents(CurrentDate.year(), CurrentDate.month());
		FirstUpdate = true;
	}
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::ReloadEvents()
{
	QDate CurrentDate = QDate::currentDate();
	ReloadEvents(CurrentDate.year(), CurrentDate.month());
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::ReloadEvents(int Year, int Month)
{
	ISTcpQuery qGetCalendarEvents(API_GET_CALENDAR_EVENTS);
	qGetCalendarEvents.BindValue("Month", Month);
	qGetCalendarEvents.BindValue("Year", Year);

	ISGui::SetWaitGlobalCursor(true);
	bool Result = qGetCalendarEvents.Execute();
	ISGui::SetWaitGlobalCursor(false);

	if (Result)
	{
		EventsMap.clear();
		QVariantList ResultList = qGetCalendarEvents.TakeAnswer()["Result"].toList();
		for (const QVariant &Variant : ResultList)
		{
			QVariantMap EventMap = Variant.toMap();
			unsigned int Day = EventMap.take("Day").toUInt();
			EventsMap[Day].append(EventMap);
		}
		CalendarPanel->SetDays(ISAlgorithm::ConvertMapToKeys(EventsMap));
		SelectedDateChanged();
	}
	else
	{
		ISMessageBox::ShowCritical(this, qGetCalendarEvents.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::SelectedDateChanged()
{
	QDate Date = CalendarPanel->selectedDate();
	LabelDayNumber->setText(QString::number(Date.day()));
	LabelDayName->setText(Date == QDate::currentDate() ? QString("%1 (%2)").arg(Date.longDayName(Date.dayOfWeek())).arg(LANG("Today")) : Date.longDayName(Date.dayOfWeek()));
	LabelMonthYear->setText(QString("%1 %2").arg(Date.longMonthName(Date.month())).arg(Date.year()));
	ListWidget->Clear();

	//ѕолучаем список событий за выбранный день
	QVariantList VariantList = EventsMap[Date.day()];
	for (const QVariant &Variant : VariantList)
	{
		QVariantMap EventMap = Variant.toMap();
		ISCalendarEventItem *EventItem = new ISCalendarEventItem(EventMap["ID"].toInt(),
			EventMap["Name"].toString(),
			EventMap["Text"].toString(),
			QTime::fromString(EventMap["TimeAlert"].toString(), FORMAT_TIME_V3),
			EventMap["Closed"].toBool(), ListWidget);
		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
		ListWidgetItem->setSizeHint(EventItem->sizeHint());
		ListWidget->setItemWidget(ListWidgetItem, EventItem);
		connect(EventItem, &ISCalendarEventItem::SizeHintChanged, [=] { ListWidgetItem->setSizeHint(EventItem->sizeHint()); });
	}
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::Create()
{
	ISCalendarObjectForm *CalendarObjectForm = dynamic_cast<ISCalendarObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_New, "_Calendar"));
	CalendarObjectForm->SetFieldValue("User", CURRENT_USER_ID);
	CalendarObjectForm->SetFieldValue("Date", CalendarPanel->selectedDate());
	connect(CalendarObjectForm, &ISCalendarObjectForm::UpdateList, this, static_cast<void(ISCalendarParagraph::*)()>(&ISCalendarParagraph::ReloadEvents));
	ISGui::ShowObjectForm(CalendarObjectForm);
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::DateTo()
{
	QDate Date = ISInputDialog::GetDate(LANG("CalendarForm.CalendarDateTo"), LANG("SelectDate") + ':');
	if (Date.isValid())
	{
		CalendarPanel->setSelectedDate(Date);
	}
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::ToCurrentDate()
{
	CalendarPanel->Today();
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::ShowSettingsForm()
{
	ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_CALENDAR);
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::Edit()
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
void ISCalendarParagraph::Delete()
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidget->currentItem()));
	if (EventItem)
	{
		DeleteEvent(EventItem->GetCalendarID());
	}
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::CloseEvent()
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidget->currentItem()));
	if (EventItem)
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CalendarCloseEvent")))
		{
			ISTcpQuery qCalendarClose(API_CALENDAR_CLOSE);
			qCalendarClose.BindValue("CalendarID", EventItem->GetCalendarID());
			if (qCalendarClose.Execute())
			{
				ReloadEvents();
			}
			else
			{
				ISMessageBox::ShowCritical(this, qCalendarClose.GetErrorString());
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidgetItem));
	if (!EventItem->GetClosed())
	{
		EditEvent(EventItem->GetCalendarID());
	}
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::ItemSelectionChanged()
{
	ISCalendarEventItem *EventItem = dynamic_cast<ISCalendarEventItem*>(ListWidget->itemWidget(ListWidget->currentItem()));
	if (EventItem)
	{
		ActionEdit->setEnabled(!EventItem->GetClosed());
		ActionDelete->setEnabled(!EventItem->GetClosed());
		ActionCloseEvent->setEnabled(!EventItem->GetClosed());
	}
	else
	{
		ActionEdit->setEnabled(false);
		ActionDelete->setEnabled(false);
		ActionCloseEvent->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::EditEvent(int CalendarID)
{
	ISCalendarObjectForm *CalendarObjectForm = dynamic_cast<ISCalendarObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_Calendar", CalendarID));
	connect(CalendarObjectForm, &ISCalendarObjectForm::UpdateList, this, &ISCalendarParagraph::SelectedDateChanged);
	ISGui::ShowObjectForm(CalendarObjectForm);
}
//-----------------------------------------------------------------------------
void ISCalendarParagraph::DeleteEvent(int CalendarID)
{
	if (ISMessageBox::ShowQuestion(CalendarPanel, LANG("Message.Question.DeleteCalendarEvent")))
	{
		ISTcpQuery qCalendarDelete(API_CALENDAR_DELETE);
		qCalendarDelete.BindValue("ID", CalendarID);
		if (qCalendarDelete.Execute())
		{
			CalendarPanel->selectionChanged();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qCalendarDelete.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
