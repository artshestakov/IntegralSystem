#include "ISSubSystem.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISProcessForm.h"
#include "ISAudioPlayerForm.h"
#include "ISDialogsCommon.h"
#include "ISLabels.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISDefinesGui.h"
#include "ISMetaData.h"
#include "ISAlgorithm.h"
#include "ISFlowLayout.h"
#include "ISProgressForm.h"
#include "ISDialogsForm.h"
//-----------------------------------------------------------------------------
ISAsteriskCallsSubSystem::ISAsteriskCallsSubSystem(QWidget *parent) : ISListBaseForm("_AsteriskCalls", parent)
{
    QAction *ActionPlay = new QAction(BUFFER_ICONS("AudioPlayer"), LANG("PlayRecordCall"), GetToolBar());
    connect(ActionPlay, &QAction::triggered, this, &ISAsteriskCallsSubSystem::PlayRecordCall);
    AddAction(ActionPlay, true, true);
}
//-----------------------------------------------------------------------------
ISAsteriskCallsSubSystem::~ISAsteriskCallsSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISAsteriskCallsSubSystem::PlayRecordCall()
{
    ISTcpQuery qGetRecordCall(API_GET_RECORD_CALL);
    qGetRecordCall.BindValue("RecordID", GetObjectID());

    ISProcessForm ProcessForm(LANG("RecordCallLoading"));
    ProcessForm.show();
    if (qGetRecordCall.Execute())
    {
        ISAudioPlayerForm::Instance().SetMedia(QByteArray::fromBase64(qGetRecordCall.GetAnswer()["Data"].toByteArray()));
        ISAudioPlayerForm::Instance().show();
        ProcessForm.hide();
    }
    else
    {
        ProcessForm.hide();
        ISMessageBox::ShowWarning(this, qGetRecordCall.GetErrorString());
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISFullTextSearchSubSystem::ISFullTextSearchSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
    GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

    QHBoxLayout *LayoutTitle = new QHBoxLayout();
    GetMainLayout()->addLayout(LayoutTitle);

    EditSearch = new ISSearchEdit(this);
    EditSearch->SetFont(ISDefines::Gui::FONT_TAHOMA_10);
    LayoutTitle->addWidget(EditSearch);

    WaitWidget = new ISWaitWidget(this, false, false);
    WaitWidget->SetRoundness(70);
    WaitWidget->SetMinimumTrailOpacity(15);
    WaitWidget->SetTrailFadePercentage(70);
    WaitWidget->SetNumberOfLines(9);
    WaitWidget->SetLineLength(7);
    WaitWidget->SetLineWidth(2);
    WaitWidget->SetInnerRadius(4);
    WaitWidget->SetRevolutionsPerSecond(2);
    LayoutTitle->addWidget(WaitWidget);

    ButtonSearch = new ISPushButton(BUFFER_ICONS("Search"), LANG("Search"), this);
    ButtonSearch->setCursor(CURSOR_POINTING_HAND);
    connect(ButtonSearch, &ISPushButton::clicked, this, &ISFullTextSearchSubSystem::Search);
    LayoutTitle->addWidget(ButtonSearch);

    QHBoxLayout *LayoutLabels = new QHBoxLayout();
    GetMainLayout()->addLayout(LayoutLabels);

    LayoutLabels->addStretch();

    LabelResult = new QLabel(this);
    LayoutLabels->addWidget(LabelResult);

    Frame = ISControls::CreateHorizontalLine(this);
    Frame->setVisible(false);
    GetMainLayout()->addWidget(Frame);

    ScrollArea = new ISScrollArea(this);
    GetMainLayout()->addWidget(ScrollArea);
}
//-----------------------------------------------------------------------------
ISFullTextSearchSubSystem::~ISFullTextSearchSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISFullTextSearchSubSystem::LoadData()
{
    EditSearch->SetFocus();
}
//-----------------------------------------------------------------------------
void ISFullTextSearchSubSystem::SetSearchInProgress(bool InProgress)
{
    ISGui::SetWaitGlobalCursor(InProgress);
    EditSearch->setEnabled(!InProgress);
    ButtonSearch->setEnabled(!InProgress);
    InProgress ? WaitWidget->Start() : WaitWidget->Stop();

    if (InProgress)
    {
        while (!WidgetList.isEmpty())
        {
            delete WidgetList.takeFirst();
        }
        LabelResult->clear();
    }
    else
    {
        EditSearch->SetFocus();
        EditSearch->SelectAll();
        Frame->setVisible(true);
    }
}
//-----------------------------------------------------------------------------
void ISFullTextSearchSubSystem::Search()
{
    if (EditSearch->GetValue().toString().isEmpty())
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.SearchFieldNotValid"));
        EditSearch->BlinkRed();
        return;
    }
    SetSearchInProgress(true);

    ISTcpQuery qSearchFullText(API_SEARCH_FULL_TEXT);
    qSearchFullText.BindValue("Value", EditSearch->GetValue());
    if (!qSearchFullText.Execute())
    {
        SetSearchInProgress(false);
        ISMessageBox::ShowCritical(this, qSearchFullText.GetErrorString());
        return;
    }

    if (ScrollArea->widget()->layout())
    {
        delete ScrollArea->widget()->layout();
    }
    ScrollArea->widget()->setLayout(new QVBoxLayout());

    QVariantList ResultList = qSearchFullText.TakeAnswer()["Result"].toList();
    if (!ResultList.isEmpty()) //Поиск дал результаты
    {
        int Index = 0;
        for (const QVariant &Variant : ResultList) //Обходим результаты поиска
        {
            QVariantMap ResultMap = Variant.toMap();
            PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(ResultMap["TableName"].toString());

            ISQLabel *LabelLink = new ISQLabel(QString("%1. %2: %3").arg(++Index).arg(MetaTable->LocalName).arg(ResultMap["ObjectName"].toString()), ScrollArea);
            LabelLink->SetIsLinked(true);
            LabelLink->setSizePolicy(QSizePolicy::Maximum, LabelLink->sizePolicy().verticalPolicy());
            LabelLink->setProperty("TableName", MetaTable->Name);
            LabelLink->setProperty("ObjectID", ResultMap["ID"]);
            connect(LabelLink, &ISQLabel::Clicked, this, &ISFullTextSearchSubSystem::ClickedRecord);
            dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addWidget(LabelLink);
            WidgetList.append(LabelLink);
        }
    }
    else //Поиск не дал результатов
    {
        QLabel *LabelNotFound = new QLabel(LANG("FullSearchTextNotResult"), ScrollArea);
        ISGui::SetFontWidgetBold(LabelNotFound, true);
        dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addWidget(LabelNotFound);
        WidgetList.append(LabelNotFound);
    }
    dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addStretch();
    LabelResult->setText(LANG("Search.ResultCount") + ": " + QString::number(ResultList.size()));
    SetSearchInProgress(false);
}
//-----------------------------------------------------------------------------
void ISFullTextSearchSubSystem::ClickedRecord()
{
    ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, sender()->property("TableName").toString(), sender()->property("ObjectID").toInt()));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISInternalDirectoriesSubSystem::ISInternalDirectoriesSubSystem(QWidget *parent)
    : ISInterfaceMetaForm(parent),
    ListBaseForm(nullptr)
{
    Layout = new QHBoxLayout();
    GetMainLayout()->addLayout(Layout);

    ListWidget = new ISListWidget(this);
    ListWidget->setAlternatingRowColors(true);
    ListWidget->setCursor(CURSOR_POINTING_HAND);
    ListWidget->setSizePolicy(QSizePolicy::Maximum, ListWidget->sizePolicy().verticalPolicy());
    connect(ListWidget, &ISListWidget::itemSelectionChanged, this, &ISInternalDirectoriesSubSystem::ItemSelectionChanged);
    Layout->addWidget(ListWidget);

    ISTcpQuery qGetInternalLists(API_GET_INTERNAL_LISTS);
    if (qGetInternalLists.Execute())
    {
        QVariantList Lists = qGetInternalLists.GetAnswer()["Lists"].toList();
        for (const QVariant &ListName : Lists)
        {
            QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
            ListWidgetItem->setText(ISMetaData::Instance().GetMetaTable(ListName.toString())->LocalListName);
            ListWidgetItem->setData(Qt::UserRole, ListName);
            ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
            ListWidgetItem->setFont(ISDefines::Gui::FONT_APPLICATION);
        }
    }
    else
    {
        ISMessageBox::ShowCritical(this, qGetInternalLists.GetErrorString());
    }

    Label = new QLabel(this);
    Label->setWordWrap(true);
    Label->setText(LANG("ClickFromViewList"));
    Label->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
    Label->setFont(ISDefines::Gui::FONT_TAHOMA_14_BOLD);
    Layout->addWidget(Label, 0, Qt::AlignCenter);
}
//-----------------------------------------------------------------------------
ISInternalDirectoriesSubSystem::~ISInternalDirectoriesSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISInternalDirectoriesSubSystem::LoadData()
{

}
//-----------------------------------------------------------------------------
void ISInternalDirectoriesSubSystem::ItemSelectionChanged()
{
    ISGui::SetWaitGlobalCursor(true);
    POINTER_DELETE(Label);
    POINTER_DELETE(ListBaseForm);

    ListWidget->SetFontItems(ISDefines::Gui::FONT_APPLICATION);
    QListWidgetItem *CurrentItem = ListWidget->currentItem();
    CurrentItem->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

    ListBaseForm = new ISListBaseForm(CurrentItem->data(Qt::UserRole).toString(), this);
    connect(ListBaseForm, &ISListBaseForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
    ListBaseForm->LoadData();
    Layout->addWidget(ListBaseForm);
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISProtocolSubSystem::ISProtocolSubSystem(QWidget *parent) : ISListBaseForm("_Protocol", parent)
{

}
//-----------------------------------------------------------------------------
ISProtocolSubSystem::~ISProtocolSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISProtocolSubSystem::DoubleClickedTable(const QModelIndex &ModelIndex)
{
    Q_UNUSED(ModelIndex);
    QVariant ObjectID = GetCurrentRecordValue("ObjectID");
    if (ObjectID.isValid() && ObjectID.type() != QVariant::LastCoreType)
    {
        ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit,
            GetCurrentRecordValueDB("TableName").toString(), ObjectID.toInt()));
    }
    else
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.ProtocolNoRecord"));
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISMonitorActivitySubSystem::ISMonitorActivitySubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
    GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

    QHBoxLayout *LayoutTitle = new QHBoxLayout();
    GetMainLayout()->addLayout(LayoutTitle);

    ISPushButton *ButtonUpdate = new ISPushButton(this);
    ButtonUpdate->setText(LANG("Update"));
    ButtonUpdate->setIcon(BUFFER_ICONS("Update"));
    connect(ButtonUpdate, &ISPushButton::clicked, this, &ISMonitorActivitySubSystem::LoadData);
    LayoutTitle->addWidget(ButtonUpdate);

    LayoutTitle->addStretch();

    QHBoxLayout *Layout = new QHBoxLayout();
    GetMainLayout()->addLayout(Layout);

    ScrollArea = new ISScrollArea(this);
    ScrollArea->widget()->setLayout(new ISFlowLayout());
    ScrollArea->widget()->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
    Layout->addWidget(ScrollArea);

    QAction *ActionUpdate = new QAction(this);
    ActionUpdate->setShortcut(Qt::Key_F5);
    connect(ActionUpdate, &QAction::triggered, this, &ISMonitorActivitySubSystem::LoadData);
    addAction(ActionUpdate);
}
//-----------------------------------------------------------------------------
ISMonitorActivitySubSystem::~ISMonitorActivitySubSystem()
{

}
//-----------------------------------------------------------------------------
void ISMonitorActivitySubSystem::LoadData()
{
    while (!VectorUsers.empty())
    {
        delete ISAlgorithm::VectorTakeBack(VectorUsers);
    }

    QSize SizeWidget;
    ISTcpQuery qGetClients(API_GET_CLIENTS);
    if (qGetClients.Execute())
    {
        QVariantList Clients = qGetClients.GetAnswer()["Clients"].toList();
        for (const QVariant &Variant : Clients)
        {
            QVariantMap ClientMap = Variant.toMap();

            ISMonitorUserWidget *MonitorUserWidget = new ISMonitorUserWidget(ClientMap, ScrollArea);
            connect(MonitorUserWidget, &ISMonitorUserWidget::ShowUserCard, this, &ISMonitorActivitySubSystem::ShowUserCard);
            connect(MonitorUserWidget, &ISMonitorUserWidget::ShowProtocol, this, &ISMonitorActivitySubSystem::ShowProtocol);
            ScrollArea->widget()->layout()->addWidget(MonitorUserWidget);
            MonitorUserWidget->adjustSize();

            QSize CurrentSize = MonitorUserWidget->size();
            if (CurrentSize.width() > SizeWidget.width())
            {
                SizeWidget.setWidth(CurrentSize.width());
            }

            if (CurrentSize.height() > SizeWidget.height())
            {
                SizeWidget.setHeight(CurrentSize.height());
            }
            VectorUsers.emplace_back(MonitorUserWidget);
        }
    }
    else
    {
        ISMessageBox::ShowCritical(this, qGetClients.GetErrorString());
    }

    SizeWidget.setHeight(SizeWidget.height() + 10);
    SizeWidget.setWidth(SizeWidget.width() + 10);
    for (ISMonitorUserWidget *MonitorUserWidget : VectorUsers)
    {
        MonitorUserWidget->setMinimumSize(SizeWidget);
    }
}
//-----------------------------------------------------------------------------
void ISMonitorActivitySubSystem::ShowUserCard()
{
    ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, "_Users", sender()->property("UserID").toInt());
    connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISMonitorActivitySubSystem::LoadData);
    ISGui::ShowObjectForm(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISMonitorActivitySubSystem::ShowProtocol()
{
    ISProtocolSubSystem *ProtocolBaseListForm = new ISProtocolSubSystem();
    ProtocolBaseListForm->setWindowTitle(LANG("ProtocolUser") + ": " + sender()->property("UserName").toString());
    ProtocolBaseListForm->setWindowIcon(BUFFER_ICONS("Protocol"));
    ProtocolBaseListForm->GetTcpQuery()->AddFilter("User", sender()->property("UserID"));
    ProtocolBaseListForm->showMaximized();
    QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, ProtocolBaseListForm, &ISProtocolSubSystem::LoadData);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISServerInfoSubSystem::ISServerInfoSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
    GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

    TabWidget = new QTabWidget(this);
    GetMainLayout()->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
ISServerInfoSubSystem::~ISServerInfoSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISServerInfoSubSystem::LoadData()
{
    ISTcpQuery qGetServerInfo(API_GET_SERVER_INFO);

    ISGui::SetWaitGlobalCursor(true);
    bool Result = qGetServerInfo.Execute();
    ISGui::SetWaitGlobalCursor(false);
    if (!Result)
    {
        ISMessageBox::ShowWarning(this, qGetServerInfo.GetErrorString());
        return;
    }

    QVariantMap AnswerMap = qGetServerInfo.GetAnswer();
    QVariantMap CaratMap = AnswerMap["Carat"].toMap(),
        DatabaseMap = AnswerMap["Database"].toMap(),
        PGSettings = AnswerMap["PGSettings"].toMap();

    QWidget *WidgetCarat = new QWidget(TabWidget);
    TabWidget->addTab(WidgetCarat, LANG("ISServerInfoSubSystem.Carat.Title"));

    QFormLayout *FormLayoutCarat = new QFormLayout();
    FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.Version"), new QLabel(CaratMap["Version"].toString(), this));
    FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.StartDateTime"), new QLabel(CaratMap["StartedDateTime"].toString(), this));
    FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.Uptime"), new QLabel(CaratMap["Uptime"].toString(), this));
    FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.SizeLogs"), new QLabel(CaratMap["SizeLogs"].toString(), this));
    FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.CountClients"), new QLabel(CaratMap["CountClients"].toString(), this));
    WidgetCarat->setLayout(FormLayoutCarat);

    QWidget *WidgetDatabase = new QWidget(TabWidget);
    TabWidget->addTab(WidgetDatabase, LANG("ISServerInfoSubSystem.Database.Title"));

    QFormLayout *FormLayoutDatabase = new QFormLayout();
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeFull"), new QLabel(DatabaseMap["SizeFull"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeIndexes"), new QLabel(DatabaseMap["SizeIndexes"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeData"), new QLabel(DatabaseMap["SizeData"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.Owner"), new QLabel(DatabaseMap["Owner"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.Encoding"), new QLabel(DatabaseMap["Encoding"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.Uptime"), new QLabel(DatabaseMap["Uptime"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.PID"), new QLabel(DatabaseMap["PID"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.Version"), new QLabel(DatabaseMap["Version"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.ClusterPath"), new QLabel(DatabaseMap["ClusterPath"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeLogs"), new QLabel(DatabaseMap["SizeLogs"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountTable"), new QLabel(DatabaseMap["CountTable"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountField"), new QLabel(DatabaseMap["CountField"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountSequence"), new QLabel(DatabaseMap["CountSequence"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountIndexes"), new QLabel(DatabaseMap["CountIndexes"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountForeign"), new QLabel(DatabaseMap["CountForeign"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.RowsCount"), new QLabel(DatabaseMap["RowsCount"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.ProtocolCount"), new QLabel(DatabaseMap["ProtocolCount"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.UsersCount"), new QLabel(DatabaseMap["UsersCount"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.FilesCount"), new QLabel(DatabaseMap["FilesCount"].toString(), this));
    FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.FilesSize"), new QLabel(DatabaseMap["FilesSize"].toString(), this));
    WidgetDatabase->setLayout(FormLayoutDatabase);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISStorageFilesSubSystem::ISStorageFilesSubSystem(QWidget *parent) : ISListBaseForm("_StorageFiles", parent)
{
    GetAction(ISNamespace::ActionType::Create)->setToolTip(LANG("StorageFiles.Create"));

    QAction *ActionSave = ISControls::CreateActionSave(this);
    connect(ActionSave, &QAction::triggered, this, &ISStorageFilesSubSystem::SaveFile);
    AddAction(ActionSave, true, true);
}
//-----------------------------------------------------------------------------
ISStorageFilesSubSystem::~ISStorageFilesSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISStorageFilesSubSystem::Create()
{
    QStringList StringList = ISFileDialog::GetOpenFileNames(this);
    if (!StringList.isEmpty())
    {
        ISProgressForm ProgressForm(StringList.size(), LANG("InsertingFiles"), this);
        ProgressForm.show();

        //Обходим все выбранные файлы
        bool Result = false;
        for (const QString &FilePath : StringList)
        {
            //Открываем файл
            QFile File(FilePath);
            if (!File.open(QIODevice::ReadOnly))
            {
                ISMessageBox::ShowWarning(&ProgressForm, LANG("Message.Error.NotOpenedFile").arg(FilePath));
                continue;
            }

            //Читаем и закрываем
            QByteArray ByteArray = File.readAll();
            File.close();

            int MaxSize = SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE) * 1000 * 1024;
            if (ByteArray.size() > MaxSize)
            {
                ISMessageBox::ShowWarning(this, LANG("Message.Warning.StorageFileBigSize").arg(FilePath).arg(MaxSize));
                continue;
            }
            ByteArray = ByteArray.toBase64();

            ISTcpQuery qAddFileStorage(API_FILE_STORAGE_ADD);
            qAddFileStorage.BindValue("FileName", QFileInfo(FilePath).fileName());
            qAddFileStorage.BindValue("Data", ByteArray);

            ISGui::SetWaitGlobalCursor(true);
            Result = qAddFileStorage.Execute();
            ISGui::SetWaitGlobalCursor(false);

            if (!Result)
            {
                ISMessageBox::ShowCritical(&ProgressForm, qAddFileStorage.GetErrorString());
            }

            if (ProgressForm.WasCanceled())
            {
                if (ISMessageBox::ShowQuestion(&ProgressForm, LANG("Message.Question.StopInsertingToStorage")))
                {
                    break;
                }
                else
                {
                    ProgressForm.SetCanceled(false);
                }
            }
            ProgressForm.IncrementValue();
        }

        if (Result)
        {
            Update();
        }
    }
}
//-----------------------------------------------------------------------------
void ISStorageFilesSubSystem::CreateCopy()
{
    QString FileName = GetCurrentRecordValue("Name").toString();
    if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CreateCopyFile").arg(FileName)))
    {
        FileName = ISInputDialog::GetString(LANG("Named"), LANG("FileName") + ':', FileName);
        if (FileName.isEmpty())
        {
            return;
        }

        ISTcpQuery qFileStorageCopy(API_FILE_STORAGE_COPY);
        qFileStorageCopy.BindValue("ID", GetObjectID());
        qFileStorageCopy.BindValue("Name", FileName);

        ISGui::SetWaitGlobalCursor(true);
        bool Result = qFileStorageCopy.Execute();
        ISGui::SetWaitGlobalCursor(false);

        if (Result)
        {
            SetSelectObjectAfterUpdate(qFileStorageCopy.GetAnswer()["NewID"].toInt());
            Update();
        }
        else
        {
            ISMessageBox::ShowCritical(this, LANG("Message.Error.CreateCopyStorageFile"));
        }
    }
}
//-----------------------------------------------------------------------------
void ISStorageFilesSubSystem::SaveFile()
{
    QString Expansion = GetCurrentRecordValue("Expansion").toString(),
        Name = GetCurrentRecordValue("Name").toString();

    if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SaveFile").arg(Name)))
    {
        QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.File").arg(Expansion), Name);
        if (FilePath.isEmpty()) //Пользователь отказался от сохранения
        {
            return;
        }

        //Пытаемся открыть файл
        QFile File(FilePath);
        if (!File.open(QIODevice::WriteOnly))
        {
            ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(FilePath), File.errorString());
            return;
        }

        //Запрашиваем файл
        ISTcpQuery qFileStorageGet(API_FILE_STORAGE_GET);
        qFileStorageGet.BindValue("ID", GetObjectID());

        ISGui::SetWaitGlobalCursor(true);
        bool Result = qFileStorageGet.Execute();
        ISGui::SetWaitGlobalCursor(false);

        if (Result) //Успешно - пишем файл на диск
        {
            File.write(QByteArray::fromBase64(qFileStorageGet.TakeAnswer()["Data"].toByteArray()));
            File.close();

            //Предлагаем открыть файл
            if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.File.SavedToPath").arg(FilePath)))
            {
                if (!ISGui::OpenFile(FilePath)) //Не удалось открыть файл
                {
                    ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(Name));
                }
            }
        }
        else //Ошибка
        {
            ISMessageBox::ShowCritical(this, qFileStorageGet.GetErrorString());
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISUserGroupSubSystem::ISUserGroupSubSystem(QWidget *parent) : ISListBaseForm("_UserGroup", parent)
{
    QAction *ActionAccess = new QAction(GetToolBar());
    ActionAccess->setText(LANG("AccessRights"));
    ActionAccess->setIcon(BUFFER_ICONS("AccessRights"));
    connect(ActionAccess, &QAction::triggered, this, &ISUserGroupSubSystem::SelectedGroup);
    AddAction(ActionAccess, true, true);
}
//-----------------------------------------------------------------------------
ISUserGroupSubSystem::~ISUserGroupSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupSubSystem::SelectedGroup()
{
    ISGui::SetWaitGlobalCursor(true);
    ISUserGroupRightDialog UserGroupForm(GetObjectID(), GetCurrentRecordValue("Name").toString());
    ISGui::SetWaitGlobalCursor(false);
    UserGroupForm.Exec();
}
//-----------------------------------------------------------------------------
