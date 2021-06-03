#include "ISParagraphs.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISDialogsCommon.h"
#include "ISMetaSystemsEntity.h"
#include "ISCalendarObjectForm.h"
#include "ISTcpQuery.h"
#include "ISListBaseForm.h"
#include "ISSystemsPanel.h"
#include "ISUserRoleEntity.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISParagraphBaseForm::ISParagraphBaseForm(QWidget *parent) : QWidget(parent)
{

}
//-----------------------------------------------------------------------------
ISParagraphBaseForm::~ISParagraphBaseForm()
{

}
//-----------------------------------------------------------------------------
void ISParagraphBaseForm::Invoke()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDesktopParagraph::ISDesktopParagraph(QWidget *parent)
    : ISParagraphBaseForm(parent),
    DesktopWidget(nullptr)
{
    MainLayout = new QVBoxLayout();
    setLayout(MainLayout);

    QString DesktopFormName = ISBuffer::Instance().ConfigurationInfo.DesktopForm;
    if (DesktopFormName.isEmpty())
    {
        MainLayout->addStretch();

        QLabel *LabelLogo = new QLabel(this);
        LabelLogo->setPixmap(ISBuffer::Instance().ConfigurationInfo.LogoName.isEmpty() ?
            BUFFER_PIXMAPS("DesktopLogo") :
            QPixmap(":_" + ISBuffer::Instance().ConfigurationInfo.Name + '/' + ISBuffer::Instance().ConfigurationInfo.LogoName));
        MainLayout->addWidget(LabelLogo, 0, Qt::AlignCenter);

        if (!ISBuffer::Instance().ConfigurationInfo.LocalName.isEmpty())
        {
            QLabel *LabelLocalName = new QLabel(this);
            LabelLocalName->setText(ISBuffer::Instance().ConfigurationInfo.LocalName);
            LabelLocalName->setFont(QFont("Tahoma", 15, QFont::Bold));
            LabelLocalName->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
            MainLayout->addWidget(LabelLocalName, 0, Qt::AlignCenter);
        }
        MainLayout->addStretch();
    }
    else
    {
        if (ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_DESKTOP))
        {
            DesktopWidget = ISAlgorithm::CreatePointer<QWidget *>(DesktopFormName, Q_ARG(QWidget *, this));
            MainLayout->addWidget(DesktopWidget);
        }
        else
        {
            QLabel *Label = new QLabel(this);
            Label->setText(LANG("NotAccessSpecialDesktop"));
            Label->setFont(FONT_TAHOMA_12_BOLD);
            MainLayout->addWidget(Label, 0, Qt::AlignCenter);
        }
    }
}
//-----------------------------------------------------------------------------
ISDesktopParagraph::~ISDesktopParagraph()
{

}
//-----------------------------------------------------------------------------
void ISDesktopParagraph::Invoke()
{
    ISParagraphBaseForm::Invoke();
    if (DesktopWidget)
    {
        DesktopWidget->show();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISWorkspaceParagraph::ISWorkspaceParagraph(QWidget *parent)
    : ISParagraphBaseForm(parent),
    CentralForm(nullptr)
{
    Layout = new QVBoxLayout();
    Layout->setContentsMargins(QMargins());
    Layout->setSpacing(0);
    setLayout(Layout);

    ISSystemsPanel *SystemsPanel = new ISSystemsPanel(this);
    connect(SystemsPanel, &ISSystemsPanel::ClickedSubSystem, this, &ISWorkspaceParagraph::ClickedSubSystem);
    Layout->addWidget(SystemsPanel);

    //Заполнение систем в виджете
    for (ISMetaSystem *MetaSystem : ISMetaSystemsEntity::Instance().GetSystems())
    {
        SystemsPanel->AddSystem(MetaSystem);
    }

    TabWidget = new ISTabWidgetMain(this);
    TabWidget->setSizePolicy(QSizePolicy::Ignored, TabWidget->sizePolicy().verticalPolicy());
    connect(TabWidget, &ISTabWidgetMain::Duplicate, this, &ISWorkspaceParagraph::AddObjectForm);
    Layout->addWidget(TabWidget);

    //Если у пользователя нет доступа ни к одной из систем
    if (ISMetaSystemsEntity::Instance().GetSystems().empty())
    {
        QLabel *Label = new QLabel(TabWidget);
        Label->setText(LANG("NotAccessSystems"));
        Label->setFont(FONT_TAHOMA_12_BOLD);
        Label->setAlignment(Qt::AlignCenter);
        TabWidget->addTab(Label, QString());
    }
}
//-----------------------------------------------------------------------------
ISWorkspaceParagraph::~ISWorkspaceParagraph()
{

}
//-----------------------------------------------------------------------------
void ISWorkspaceParagraph::Invoke()
{
    ISParagraphBaseForm::Invoke();
}
//-----------------------------------------------------------------------------
void ISWorkspaceParagraph::AddObjectForm(QWidget *ObjectForm)
{
    connect(ObjectForm, &ISObjectFormBase::windowTitleChanged, [=](const QString &WindowTitle)
    {
        TabWidget->setTabText(TabWidget->indexOf(ObjectForm), WindowTitle);
    });
    connect(ObjectForm, &ISObjectFormBase::windowIconChanged, [=](const QIcon &WindowIcon)
    {
        TabWidget->setTabIcon(TabWidget->indexOf(ObjectForm), WindowIcon);
    });
    connect(dynamic_cast<ISObjectFormBase*>(ObjectForm), &ISObjectFormBase::CurrentObjectTab, [=]
    {
        TabWidget->setCurrentWidget(ObjectForm);
    });
    TabWidget->addTab(ObjectForm, ObjectForm->windowIcon(), ObjectForm->windowTitle());
    TabWidget->setCurrentWidget(ObjectForm);
}
//-----------------------------------------------------------------------------
void ISWorkspaceParagraph::ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem)
{
    if (SubSystemUID == CurrentSubSystemUID)
    {
        TabWidget->setCurrentIndex(0);
        return;
    }
    CurrentSubSystemUID = SubSystemUID;

    ISGui::SetWaitGlobalCursor(true);
    ISMetaSubSystem *MetaSubSystem = ISMetaSystemsEntity::Instance().GetSubSystem(SubSystemUID);
    POINTER_DELETE(CentralForm);
    if (!MetaSubSystem->TableName.isEmpty()) //Открытие таблицы
    {
        CentralForm = new ISListBaseForm(MetaSubSystem->TableName, this);
    }
    else if (!MetaSubSystem->ClassName.isEmpty()) //Открытие класса (виджета)
    {
        CentralForm = ISAlgorithm::CreatePointer<ISInterfaceMetaForm *>(MetaSubSystem->ClassName, Q_ARG(QWidget *, this));
    }
    connect(CentralForm, &ISListBaseForm::AddFormFromTab, this, &ISWorkspaceParagraph::AddObjectForm);
    TabWidget->insertTab(0, CentralForm, IconSubSystem, MetaSubSystem->LocalName);
    TabWidget->setCurrentIndex(0);
    ISGui::SetWaitGlobalCursor(false);
    QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, Qt::PreciseTimer, CentralForm, &ISInterfaceMetaForm::LoadData);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
    LayoutSelectedDate->setContentsMargins(QMargins());
    LayoutRight->addLayout(LayoutSelectedDate);

    QDate CurrentDate = QDate::currentDate();

    LabelDayNumber = new QLabel(QString::number(CurrentDate.day()), Panel);
    LabelDayNumber->setFont(QFont("Tahoma", 35));
    LayoutSelectedDate->addWidget(LabelDayNumber, 0, Qt::AlignVCenter);

    QVBoxLayout *LayoutSelectedDateRight = new QVBoxLayout();
    LayoutSelectedDate->addLayout(LayoutSelectedDateRight);

    LabelDayName = new QLabel(CurrentDate == QDate::currentDate() ?
        QString("%1 (%2)").arg(CurrentDate.longDayName(CurrentDate.dayOfWeek())).arg(LANG("Today")) :
        CurrentDate.longDayName(CurrentDate.dayOfWeek()), this);
    LabelDayName->setFont(QFont("Tahoma", 18));
    LayoutSelectedDateRight->addWidget(LabelDayName);

    LabelMonthYear = new QLabel(QString("%1 %2").arg(CurrentDate.longMonthName(CurrentDate.month())).arg(CurrentDate.year()), this);
    LabelMonthYear->setFont(QFont("Tahoma", 12));
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

    //Получаем список событий за выбранный день
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
    ISCalendarObjectForm *CalendarObjectForm = dynamic_cast<ISCalendarObjectForm*>(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::New, "_Calendar"));
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
    ISCalendarObjectForm *CalendarObjectForm = dynamic_cast<ISCalendarObjectForm*>(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, "_Calendar", CalendarID));
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
