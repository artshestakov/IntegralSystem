#include "ISDialogsForm.h"
#include "ISLocalizationOld.h"
#include "ISBuffer.h"
#include "ISDialogsCommon.h"
#include "ISConfig.h"
#include "ISGui.h"
#include "ISTcpConnector.h"
#include "ISControls.h"
#include "ISSettings.h"
#include "ISAlgorithmOld.h"
#include "ISPopupMessage.h"
#include "ISMetaData.h"
#include "ISProcessForm.h"
#include "ISProperty.h"
#include "ISRevision.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISAboutDialog::ISAboutDialog() : ISInterfaceDialogForm()
{
    setWindowTitle(LANG("AboutForm.AboutApplication"));
    setMinimumSize(SIZE_MAIN_WINDOW_MINIMUM);
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    QHBoxLayout *Layout = new QHBoxLayout();
    GetMainLayout()->addLayout(Layout);

    QPixmap PixmapLogo = BUFFER_PIXMAPS("Logo");
    QPixmap NewPixmap = PixmapLogo.scaled(QSize(200, 200), Qt::IgnoreAspectRatio, Qt::FastTransformation);

    QLabel *LabelImage = new QLabel(this);
    LabelImage->setPixmap(NewPixmap);
    Layout->addWidget(LabelImage);

    LayoutRight = new QVBoxLayout();
    Layout->addLayout(LayoutRight);

    TabWidget = new QTabWidget(this);
    LayoutRight->addWidget(TabWidget);

    QHBoxLayout *LayoutBottom = new QHBoxLayout();
    GetMainLayout()->addLayout(LayoutBottom);

    ISPushButton *ButtonFeedback = new ISPushButton(BUFFER_ICONS("Feedback"), LANG("AboutForm.Feedback"), this);
    connect(ButtonFeedback, &ISPushButton::clicked, this, &ISAboutDialog::Feedback);
    LayoutBottom->addWidget(ButtonFeedback);

    LayoutBottom->addStretch();

    ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
    connect(ButtonClose, &ISPushButton::clicked, this, &ISAboutDialog::close);
    LayoutBottom->addWidget(ButtonClose);

    CreateCommonTab();
    CreateContactsTab();
    CreateModuleTab();
    CreateDescriptionTab();
    CreateLicenseTab();
    CreateOtherTab();
}
//-----------------------------------------------------------------------------
ISAboutDialog::~ISAboutDialog()
{

}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateCommonTab()
{
    QVBoxLayout *LayoutCommon = new QVBoxLayout();

    QWidget *TabCommon = new QWidget(TabWidget);
    TabCommon->setLayout(LayoutCommon);
    TabWidget->addTab(TabCommon, LANG("AboutForm.Tab.Common"));

    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ProductName"), QCoreApplication::applicationName());
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Cofiguration"), CARAT_CONFIGURATION);
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Platform"), CARAT_PLATFORM);
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Version"), CARAT_VERSION);
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.ReleaseDateTime"), QString("%1 %2").arg(CARAT_BUILD_DATE).arg(CARAT_BUILD_TIME));
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Hash"), CARAT_HASH);
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Branch"), CARAT_BRANCH_NAME);
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.QtVersion"), qVersion());
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Server"), CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER));
    AddLabel(TabCommon, LANG("AboutForm.Tab.Common.Port"), CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT));

    LayoutCommon->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateContactsTab()
{
    QVBoxLayout *LayoutContacts = new QVBoxLayout();

    QWidget *TabContacts = new QWidget(TabWidget);
    TabContacts->setLayout(LayoutContacts);
    TabWidget->addTab(TabContacts, LANG("AboutForm.Tab.Contacts"));

    AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthor"), LANG("AboutForm.Tab.Contacts.ProductAuthor.FullName"));
    AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthorPhone"), "+7 (918) 412-55-10");
    AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.Telegram"), "@artem_shestakov");
    AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.ProductAuthorMail"), "art.shestakov@icloud.com");
    AddLabel(TabContacts, LANG("AboutForm.Tab.Contacts.Address"), LANG("AboutForm.Tab.Contacts.Address.Text"));

    LayoutContacts->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateModuleTab()
{
    if (ISBuffer::Instance().ConfigurationInfo.Name.isEmpty())
    {
        return;
    }

    QVBoxLayout *LayoutModule = new QVBoxLayout();

    QWidget *TabModule = new QWidget(TabWidget);
    TabModule->setLayout(LayoutModule);
    TabWidget->addTab(TabModule, LANG("AboutForm.Tab.Configuration"));

    AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.Name"), ISBuffer::Instance().ConfigurationInfo.Name);
    AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.LocalName"), ISBuffer::Instance().ConfigurationInfo.LocalName);
    AddLabel(TabModule, LANG("AboutForm.Tab.Configuration.UID"), ISBuffer::Instance().ConfigurationInfo.UID);

    LayoutModule->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateDescriptionTab()
{
    QVBoxLayout *LayoutDescription = new QVBoxLayout();

    QWidget *TabDescription = new QWidget(TabWidget);
    TabDescription->setLayout(LayoutDescription);
    TabWidget->addTab(TabDescription, LANG("AboutForm.Tab.Description"));

    ISTextEdit *TextEdit = new ISTextEdit(TabDescription);
    TextEdit->SetValue(LANG("DescriptionApplication"));
    TextEdit->SetReadOnly(true);
    TextEdit->SetSizePolicyVertical(QSizePolicy::Minimum);
    TabDescription->layout()->addWidget(TextEdit);
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateLicenseTab()
{
    QVBoxLayout *LayoutLicense = new QVBoxLayout();

    QWidget *TabLicense = new QWidget(TabWidget);
    TabLicense->setLayout(LayoutLicense);
    TabWidget->addTab(TabLicense, LANG("AboutForm.Tab.License"));

    ISTextEdit *TextEdit = new ISTextEdit(TabLicense);
    TextEdit->SetReadOnly(true);
    TextEdit->SetSizePolicyVertical(QSizePolicy::Minimum);
    TabLicense->layout()->addWidget(TextEdit);

    QFile FileLicense(":Licenses/IntegralSystem.txt");
    if (FileLicense.open(QIODevice::ReadOnly))
    {
        TextEdit->SetValue(QString::fromLocal8Bit(FileLicense.readAll()));
        FileLicense.close();
    }
}
//-----------------------------------------------------------------------------
void ISAboutDialog::CreateOtherTab()
{
    QVBoxLayout *LayoutOther = new QVBoxLayout();

    QWidget *TabOther = new QWidget(TabWidget);
    TabOther->setLayout(LayoutOther);
    TabWidget->addTab(TabOther, LANG("AboutForm.Tab.Other"));

    AddLabel(TabOther, LANG("AboutForm.Tab.Other.ApplicationDir"), QCoreApplication::applicationDirPath());

    LayoutOther->addStretch();
}
//-----------------------------------------------------------------------------
void ISAboutDialog::AddLabel(QWidget *parent, const QString &LabelText, const QString &Text)
{
    QHBoxLayout *LayoutRow = new QHBoxLayout();
    LayoutRow->setContentsMargins(QMargins());

    QWidget *WidgetRow = new QWidget(parent);
    WidgetRow->setLayout(LayoutRow);

    QLabel *LabelRow = new QLabel(parent);
    LabelRow->setFont(FONT_APPLICATION_BOLD);
    LabelRow->setText(LabelText + ':');
    LayoutRow->addWidget(LabelRow);

    QLabel *Label = new QLabel(parent);
    Label->setFont(FONT_APPLICATION_BOLD);
    Label->setText(Text);
    LayoutRow->addWidget(Label);

    LayoutRow->addStretch();

    parent->layout()->addWidget(WidgetRow);
}
//-----------------------------------------------------------------------------
void ISAboutDialog::Feedback()
{
    ISGui::SetWaitGlobalCursor(true);
    bool Opened = QDesktopServices::openUrl(QUrl(LINK_FEEDBACK));
    ISGui::SetWaitGlobalCursor(false);
    if (!Opened)
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotOpenFeedbackLink"), LINK_FEEDBACK);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISAuthDialog::ISAuthDialog()
    : ISInterfaceDialogForm(true),
    ConnectingState(false)
{
    setWindowTitle(LANG("InputInSystem"));
    ForbidResize();
    GetMainLayout()->setContentsMargins(QMargins());

    QLabel *LableImage = new QLabel(this);
    LableImage->setScaledContents(true);
    LableImage->setPixmap(BUFFER_PIXMAPS("BannerLogoAuth"));
    GetMainLayout()->addWidget(LableImage);

    QVBoxLayout *LayoutFields = new QVBoxLayout();
    LayoutFields->setContentsMargins(QMargins(10, 10, 10, 10));
    GetMainLayout()->addLayout(LayoutFields);

    LayoutFields->addWidget(new QLabel(LANG("InputLoginAndPassword") + ':', this));

    EditLogin = new ISLineEdit(this);
    EditLogin->SetPlaceholderText(LANG("Login"));
    EditLogin->SetIcon(BUFFER_ICONS("Auth.Login"));
    EditLogin->SetRegExp(REG_EXP_LOGIN);
    LayoutFields->addWidget(EditLogin);

    EditPassword = new ISLineEdit(this);
    EditPassword->SetPlaceholderText(LANG("Password"));
    EditPassword->SetIcon(BUFFER_ICONS("Auth.Password"));
    EditPassword->SetEchoMode(QLineEdit::Password);
    LayoutFields->addWidget(EditPassword);

    CheckRememberUser = new ISCheckEdit(this);
    CheckRememberUser->SetText(LANG("RememberMe"));
    CheckRememberUser->SetToolTip(LANG("RememberMe.ToolTip"));
    CheckRememberUser->SetValue(CONFIG_BOOL("RememberUser/Include"));
    LayoutFields->addWidget(CheckRememberUser);

    LayoutFields->addSpacerItem(new QSpacerItem(0, 55));

    LabelIndicator = new QLabel(this);
    LayoutFields->addWidget(LabelIndicator, 0, Qt::AlignRight);

    QHBoxLayout *LayoutBottom = new QHBoxLayout();
    LayoutFields->addLayout(LayoutBottom);

    ButtonMenu = new ISServiceButton(BUFFER_ICONS("Auth.Additionally"), LANG("Additionally"), this);
    ButtonMenu->setMenu(new QMenu(ButtonMenu));
    LayoutBottom->addWidget(ButtonMenu);

    ButtonMenu->menu()->addAction(BUFFER_ICONS("DatabaseConnection"), LANG("Form.Authorization.ConnectionSettings"), this, &ISAuthDialog::ShowConnectionForm, Qt::Key_F9);
    ButtonMenu->menu()->addAction(BUFFER_ICONS("About"), LANG("Form.Authorization.About"), this, &ISAuthDialog::ShowAboutForm, Qt::Key_F1);

    LayoutBottom->addStretch();

    WaitWidget = new ISWaitWidget(this, false, false);
    WaitWidget->SetRoundness(70);
    WaitWidget->SetMinimumTrailOpacity(15);
    WaitWidget->SetTrailFadePercentage(70);
    WaitWidget->SetNumberOfLines(9);
    WaitWidget->SetLineLength(7);
    WaitWidget->SetLineWidth(2);
    WaitWidget->SetInnerRadius(4);
    WaitWidget->SetRevolutionsPerSecond(2);
    LayoutBottom->addWidget(WaitWidget);

    ButtonInput = new ISPushButton(BUFFER_ICONS("Apply.Blue"), LANG("Input"), this);
    ButtonInput->setToolTip(LANG("Input.ToolTip"));
    ButtonInput->setCursor(CURSOR_POINTING_HAND);
    connect(ButtonInput, &ISPushButton::clicked, this, &ISAuthDialog::Input);
    LayoutBottom->addWidget(ButtonInput);

    ButtonExit = new ISPushButton(BUFFER_ICONS("Auth.Exit"), LANG("Exit"), this);
    ButtonExit->setCursor(CURSOR_POINTING_HAND);
    ButtonExit->setToolTip(LANG("Exit.ToolTip"));
    connect(ButtonExit, &ISPushButton::clicked, this, &ISAuthDialog::close);
    LayoutBottom->addWidget(ButtonExit);

#ifdef DEBUG
    EditLogin->SetValue(SYSTEM_USER_LOGIN);
    EditPassword->SetValue("adm777");
#else
    if (CheckRememberUser->GetValue().toBool())
    {
        EditLogin->SetValue(CONFIG_STRING("RememberUser/Login"));
        QTimer::singleShot(100, EditPassword, &ISLineEdit::SetFocus);
    }
#endif
}
//-----------------------------------------------------------------------------
ISAuthDialog::~ISAuthDialog()
{

}
//-----------------------------------------------------------------------------
void ISAuthDialog::closeEvent(QCloseEvent *CloseEvent)
{
    ConnectingState ? CloseEvent->ignore() : ISInterfaceDialogForm::closeEvent(CloseEvent);
}
//-----------------------------------------------------------------------------
void ISAuthDialog::EnterClicked()
{
    Input();
}
//-----------------------------------------------------------------------------
void ISAuthDialog::ShowConnectionForm()
{
    ISConnectDialog().Exec();
}
//-----------------------------------------------------------------------------
void ISAuthDialog::ShowAboutForm()
{
    ISAboutDialog().Exec();
}
//-----------------------------------------------------------------------------
void ISAuthDialog::Input()
{
    //Проверяем заполнение конфигурационного файла
    if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
    {
        ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionSetting.Input.ServerEmpty"));
        return;
    }
    if (CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT).isEmpty())
    {
        ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionSetting.Input.PortEmpty"));
        return;
    }
    if (EditLogin->GetValue().toString().isEmpty())
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Error.Input.LoginEmpty"));
        EditLogin->BlinkRed();
        return;
    }
    if (EditPassword->GetValue().toString().isEmpty())
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Error.Input.PasswordEmpty"));
        EditPassword->BlinkRed();
        return;
    }

    //Для запоминания пользователя
    ISConfig::Instance().SetValue("RememberUser/Include", CheckRememberUser->GetValue().toBool());
    ISConfig::Instance().SetValue("RememberUser/Login", EditLogin->GetValue().toString());

    SetConnecting(true);
    if (!ISTcpConnector::Instance().Connect(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT))) //Ошибка подключения к карату
    {
        SetConnecting(false);
        ISMessageBox MessageBox(ISMessageBox::Question, LANG("Question"),
            LANG("Message.Question.ConnectionError.Reconnect").arg(ISTcpConnector::Instance().GetErrorString()), QString(),
            {
                { 1, LANG("Yes") },
                { 2, LANG("No"), true }, //Нажатие на эту кнопку не учитывается
                { 3, LANG("Exit") }
            }, this);
        switch (MessageBox.Exec())
        {
        case 1: Input(); break; //Выбрали повторить подключение
        case 3: close(); break; //Выбрали выход из программы
        }
        return;
    }
    LabelIndicator->setText(LANG("AuthProcess"));

    ISTcpQuery qAuth(API_AUTH);
    qAuth.BindValue("Hash", ISAlgorithm::StringToSHA256(EditLogin->GetValue().toString().toStdString() + EditPassword->GetValue().toString().toStdString()).c_str());
    qAuth.BindValue("Version", CARAT_VERSION_N);
    if (qAuth.Execute()) //Авторизация прошла успешно
    {
        QVariantMap AnswerMap = qAuth.GetAnswer(); //Получаем ответ

        QVariantMap AlreadyConnected = AnswerMap.contains("AlreadyConnected") ? AnswerMap["AlreadyConnected"].toMap() : QVariantMap();
        if (!AlreadyConnected.isEmpty()) //Если клиент уже подключен - сообщаем об этом
        {
            ISGui::SetWaitGlobalCursor(false);
            ISMessageBox::ShowWarning(this, LANG("Message.Warning.AlreadyConnected").arg(AlreadyConnected["DateTime"].toString()).arg(AlreadyConnected["IPAddress"].toString()));
            ISGui::SetWaitGlobalCursor(true);
        }

        QVariantMap UpdateClientMap = AnswerMap["UpdateClient"].toMap();
        ISProperty::Instance().SetServerVersion(AnswerMap["Server"].toMap()["Version"].toUInt());

        if (UpdateClientMap["IsNeed"].toBool()) //Если требуется обновление - предлагаем скачать и установить
        {
            SetConnecting(false);
            if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.UpdateAvailable")
                .arg(CARAT_VERSION_N).arg(UpdateClientMap["NewVersion"].toUInt()))) //Пользователь согласился
            {
                ISProcessForm ProcessForm(LANG("UploadingUpdate"), this);
                ProcessForm.show();
                if (qAuth.Execute(API_GET_LAST_CLIENT)) //Запрос на обновление прошёл успешно
                {
                    ProcessForm.SetText(LANG("SavingUpdate"));
                    AnswerMap = qAuth.GetAnswer();
                    QFile File(QCoreApplication::applicationDirPath() + "/Temp/" + AnswerMap["FileName"].toString());
                    if (File.open(QIODevice::WriteOnly))
                    {
                        File.write(QByteArray::fromBase64(AnswerMap["Data"].toByteArray()));
                        File.close();
                        ProcessForm.hide();
                        ISMessageBox::ShowInformation(this, LANG("Message.Information.InstallUpdate"));
                        if (!QProcess::startDetached(File.fileName(), QStringList() << "/SILENT" << "/NOCANCEL" << "/NORESTART")) //Не удалось запустить установку
                        {
                            ISMessageBox::ShowWarning(this, LANG("Message.Warning.StartInstallUpdate").arg(File.fileName()));
                        }
                        close();
                        return;
                    }
                    else //Не удалось сохранить обновление
                    {
                        SetConnecting(false);
                        ISMessageBox::ShowCritical(&ProcessForm, LANG("Message.Error.SaveUpdate"), File.errorString());
                    }
                }
                else //Ошибка запроса на обновление
                {
                    SetConnecting(false);
                    ISMessageBox::ShowCritical(&ProcessForm, LANG("Message.Error.DownloadUpdate"), qAuth.GetErrorString());
                }
            }
        }

        //Получаем информацию о текущем пользователе
        ISBuffer::Instance().CurrentUserInfo.System = AnswerMap["UserIsSystem"].toBool();
        ISBuffer::Instance().CurrentUserInfo.ID = AnswerMap["UserID"].toUInt();
        ISBuffer::Instance().CurrentUserInfo.FIO = AnswerMap["UserFIO"].toString();
        ISBuffer::Instance().CurrentUserInfo.Login = EditLogin->GetValue().toString();
        ISBuffer::Instance().CurrentUserInfo.Password = EditPassword->GetValue().toString();
        ISBuffer::Instance().CurrentUserInfo.GroupID = AnswerMap["UserGroupID"].toUInt();
        ISBuffer::Instance().CurrentUserInfo.GroupFullAccess = AnswerMap["UserGroupFullAccess"].toBool();

        //Получаем информацию о конфигурации
        QVariantMap ConfigurationInfo = AnswerMap["Configuration"].toMap();
        ISBuffer::Instance().ConfigurationInfo.Name = ConfigurationInfo["Name"].toString();
        ISBuffer::Instance().ConfigurationInfo.UID = ConfigurationInfo["UID"].toString();
        ISBuffer::Instance().ConfigurationInfo.LocalName = ConfigurationInfo["LocalName"].toString();
        ISBuffer::Instance().ConfigurationInfo.DesktopForm = ConfigurationInfo["DesktopForm"].toString();
        //???
        //ISBuffer::Instance().ConfigurationInfo.DateExpired = QDate::fromString(ConfigurationInfo["DateExpired"].toString(), FORMAT_DATE_V2);
        ISBuffer::Instance().ConfigurationInfo.LogoName = ConfigurationInfo["LogoName"].toString();

        SetConnecting(false);
        SetResult(true);
        hide();
        close();
    }
    else //Ошибка авторизации
    {
        SetConnecting(false);
        ISMessageBox::ShowCritical(this, LANG("Message.Error.Auth"), qAuth.GetErrorString());
    }
}
//-----------------------------------------------------------------------------
void ISAuthDialog::SetConnecting(bool Connecting)
{
    ConnectingState = Connecting;
    ConnectingState ? WaitWidget->Start() : WaitWidget->Stop();
    ConnectingState ? LabelIndicator->setText(LANG("ConnectingToServer")) : LabelIndicator->clear();
    ISGui::SetWaitGlobalCursor(ConnectingState);
    EditLogin->setEnabled(!ConnectingState);
    EditPassword->setEnabled(!ConnectingState);
    CheckRememberUser->setEnabled(!ConnectingState);
    ButtonMenu->setEnabled(!ConnectingState);
    ButtonInput->setEnabled(!ConnectingState);
    ButtonExit->setEnabled(!ConnectingState);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISConnectDialog::ISConnectDialog() : ISInterfaceDialogForm()
{
    setWindowTitle(LANG("ConnectionSettings"));
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));
    resize(650, 300);

    QFormLayout *FormLayout = new QFormLayout();
    GetMainLayout()->addLayout(FormLayout);

    EditServer = new ISLineEdit(this);
    EditServer->SetValue(CONFIG_VALUE(CONST_CONFIG_CONNECTION_SERVER));
    FormLayout->addRow(LANG("Server") + ':', EditServer);

    EditPort = new ISIntegerEdit(this);
    EditPort->SetValue(CONFIG_VALUE(CONST_CONFIG_CONNECTION_PORT));
    FormLayout->addRow(LANG("Port") + ':', EditPort);

    QHBoxLayout *LayoutButtons = new QHBoxLayout();
    LayoutButtons->addStretch();
    GetMainLayout()->addLayout(LayoutButtons);

    ISPushButton *ButtonSave = new ISPushButton(BUFFER_ICONS("Save"), LANG("Save"), this);
    connect(ButtonSave, &ISPushButton::clicked, this, &ISConnectDialog::Save);
    LayoutButtons->addWidget(ButtonSave, 0, Qt::AlignRight);

    ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
    connect(ButtonClose, &ISPushButton::clicked, this, &ISConnectDialog::close);
    LayoutButtons->addWidget(ButtonClose);
}
//-----------------------------------------------------------------------------
ISConnectDialog::~ISConnectDialog()
{

}
//-----------------------------------------------------------------------------
void ISConnectDialog::AfterShowEvent()
{
    EditServer->SetFocus();
}
//-----------------------------------------------------------------------------
void ISConnectDialog::EnterClicked()
{
    Save();
}
//-----------------------------------------------------------------------------
void ISConnectDialog::Save()
{
    ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_SERVER, EditServer->GetValue());
    ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_PORT, EditPort->GetValue());
    close();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExportDialog::ISExportDialog(ISTcpModel *TcpModel)
    : ISInterfaceDialogForm(),
    SelectedType(ISNamespace::ExportType::Unknown)
{
    setWindowIcon(BUFFER_ICONS("ExportTable"));
    setWindowTitle(LANG("Export.Table"));
    setFixedSize(QSize(640, 480));
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    QLabel *LabelText = new QLabel(this);
    LabelText->setText(LANG("SelectExportType") + ':');
    GetMainLayout()->addWidget(LabelText);

    ComboBoxType = new ISComboEdit(this);
    ComboBoxType->SetEditable(false);
    ComboBoxType->AddItem(LANG("NotSelected"), static_cast<int>(ISNamespace::ExportType::Unknown));
    ComboBoxType->AddItem(LANG("Export.Type.CSV"), static_cast<int>(ISNamespace::ExportType::CSV));
    ComboBoxType->AddItem(LANG("Export.Type.HTML"), static_cast<int>(ISNamespace::ExportType::HTML));
    ComboBoxType->AddItem(LANG("Export.Type.XML"), static_cast<int>(ISNamespace::ExportType::XML));
    ComboBoxType->AddItem(LANG("Export.Type.JSON"), static_cast<int>(ISNamespace::ExportType::JSON));
    connect(ComboBoxType, &ISComboEdit::ValueChange, this, &ISExportDialog::TypeChanged);
    GetMainLayout()->addWidget(ComboBoxType);

    TabWidget = new QTabWidget(this);
    GetMainLayout()->addWidget(TabWidget);

    CreateTabSettings();
    CreateTabFields(TcpModel);

    ButtonDialog = new ISButtonDialog(this, LANG("Export"));
    ButtonDialog->SetApplyEnabled(false);
    connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISExportDialog::Select);
    connect(ButtonDialog, &ISButtonDialog::Close, this, &ISExportDialog::close);
    GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISExportDialog::~ISExportDialog()
{

}
//-----------------------------------------------------------------------------
ISNamespace::ExportType ISExportDialog::GetSelectedType()
{
    return SelectedType;
}
//-----------------------------------------------------------------------------
QString ISExportDialog::GetSelectTypeName() const
{
    return ComboBoxType->GetCurrentText();
}
//-----------------------------------------------------------------------------
ISVectorUInt ISExportDialog::GetSelectedFields() const
{
    return SelectedFields;
}
//-----------------------------------------------------------------------------
bool ISExportDialog::GetHeader() const
{
    return CheckHeader->GetValue().toBool();
}
//-----------------------------------------------------------------------------
void ISExportDialog::CreateTabSettings()
{
    QFormLayout *FormLayout = new QFormLayout();

    QWidget *TabSettings = new QWidget(TabWidget);
    TabSettings->setLayout(FormLayout);
    TabWidget->addTab(TabSettings, LANG("Export.Settings"));

    CheckHeader = new ISCheckEdit(TabSettings);
    CheckHeader->SetValue(true);
    FormLayout->addRow(LANG("Export.Setting.Header") + ':', CheckHeader);
}
//-----------------------------------------------------------------------------
void ISExportDialog::CreateTabFields(ISTcpModel *TcpModel)
{
    QVBoxLayout *LayoutFields = new QVBoxLayout();

    QWidget *TabFields = new QWidget(TabWidget);
    TabFields->setLayout(LayoutFields);
    TabWidget->addTab(TabFields, LANG("Export.Fields"));

    QToolBar *ToolBar = new QToolBar(TabFields);
    ToolBar->setIconSize(QSize(22, 22));
    LayoutFields->addWidget(ToolBar);

    QAction *ActionSelectAll = new QAction(BUFFER_ICONS("CheckAll"), LANG("SelectAllFields"), ToolBar);
    connect(ActionSelectAll, &QAction::triggered, [=] { ListFields->SetCheckedItems(true); });
    ToolBar->addAction(ActionSelectAll);

    QAction *ActionDeselectAll = new QAction(BUFFER_ICONS("CheckNotAll"), LANG("DeselectAllFields"), ToolBar);
    connect(ActionDeselectAll, &QAction::triggered, [=] { ListFields->SetCheckedItems(false); });
    ToolBar->addAction(ActionDeselectAll);

    ListFields = new ISListWidget(TabFields);
    ListFields->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ListFields->setDragEnabled(true);
    ListFields->setAcceptDrops(true);
    ListFields->setDropIndicatorShown(true);
    ListFields->setDefaultDropAction(Qt::MoveAction);
    LayoutFields->addWidget(ListFields);

    for (unsigned int i = 0, c = TcpModel->columnCount(); i < c; ++i)
    {
        ISModelField ModelField = TcpModel->GetField(i);
        if (!ModelField.IsSystem)
        {
            CreateFieldItem(TcpModel->GetField(i));
        }
    }
    connect(ListFields, &QListWidget::itemChanged, this, &ISExportDialog::FieldsPositionChanged);
    connect(ListFields, &QListWidget::itemDoubleClicked, this, &ISExportDialog::ItemDoubleClicked);
    connect(ListFields->model(), &QAbstractListModel::rowsMoved, this, &ISExportDialog::FieldsPositionChanged);
}
//-----------------------------------------------------------------------------
void ISExportDialog::Select()
{
    if (SelectedFields.empty())
    {
        ISMessageBox::ShowWarning(this, LANG("Export.NotSelectedFields"));
        return;
    }
    SetResult(true);
    close();
}
//-----------------------------------------------------------------------------
void ISExportDialog::CreateFieldItem(const ISModelField &ModelField)
{
    QListWidgetItem *FieldItem = new QListWidgetItem(ListFields);
    FieldItem->setText(ModelField.LocalName);
    FieldItem->setData(Qt::UserRole, ModelField.Index);
    FieldItem->setCheckState(Qt::Checked);
    FieldItem->setSizeHint(QSize(FieldItem->sizeHint().width(), 30));
    SelectedFields.emplace_back((unsigned int)ModelField.Index);
}
//-----------------------------------------------------------------------------
void ISExportDialog::FieldsPositionChanged()
{
    SelectedFields.clear();
    for (int i = 0; i < ListFields->count(); ++i)
    {
        QListWidgetItem *ListWidgetItem = ListFields->item(i);
        if (!ListWidgetItem->text().isEmpty())
        {
            if (ListWidgetItem->checkState() == Qt::Checked)
            {
                SelectedFields.emplace_back(ListWidgetItem->data(Qt::UserRole).toUInt());
            }
        }
    }
}
//-----------------------------------------------------------------------------
void ISExportDialog::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
    ListWidgetItem->checkState() == Qt::Checked ?
        ListWidgetItem->setCheckState(Qt::Unchecked) :
        ListWidgetItem->setCheckState(Qt::Checked);
}
//-----------------------------------------------------------------------------
void ISExportDialog::EnterClicked()
{
    Select();
}
//-----------------------------------------------------------------------------
void ISExportDialog::TypeChanged(const QVariant &Value)
{
    SelectedType = static_cast<ISNamespace::ExportType>(Value.toInt());
    ButtonDialog->SetApplyEnabled(static_cast<ISNamespace::ExportType>(Value.toInt()) != ISNamespace::ExportType::Unknown);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISReconnectDialog::ISReconnectDialog()
    : ISInterfaceDialogForm(true),
    Attempts(0)
{
    setWindowTitle(LANG("ReconnectForm.Title"));
    setWindowIcon(BUFFER_ICONS("Update"));
    setCursor(CURSOR_BUSY);
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));
    resize(300, 165);

    QLabel *LabelInfo = new QLabel(LANG("ReconnectForm.Info"), this);
    LabelInfo->setWordWrap(true);
    GetMainLayout()->addWidget(LabelInfo);

    GetMainLayout()->addWidget(new QLabel(LANG("ReconnectForm.DateTime").arg(QDateTime::currentDateTime().toString(FORMAT_DATE_TIME_V2)), this));

    LabelAttempts = new QLabel(LANG("ReconnectForm.Attempts").arg(Attempts), this);
    GetMainLayout()->addWidget(LabelAttempts);

    QProgressBar *ProgressBar = new QProgressBar(this);
    ProgressBar->setValue(0);
    ProgressBar->setRange(0, 0);
    ProgressBar->setTextVisible(false);
    GetMainLayout()->addWidget(ProgressBar);

    Timer = new QTimer(this);
    connect(Timer, &QTimer::timeout, this, &ISReconnectDialog::Timeout);
    Timer->setInterval(CARAT_RECONNECT_TIMEOUT);
    Timer->setSingleShot(true);
    Timer->start();

    GetMainLayout()->addStretch();

    QHBoxLayout *LayoutBottom = new QHBoxLayout();
    LayoutBottom->addStretch();
    GetMainLayout()->addLayout(LayoutBottom);

    ISPushButton *ButtonExit = new ISPushButton(BUFFER_ICONS("Exit"), LANG("ExitApplication"), this);
    connect(ButtonExit, &ISPushButton::clicked, this, &ISReconnectDialog::close);
    LayoutBottom->addWidget(ButtonExit);
}
//-----------------------------------------------------------------------------
ISReconnectDialog::~ISReconnectDialog()
{

}
//-----------------------------------------------------------------------------
void ISReconnectDialog::Timeout()
{
    if (ISTcpConnector::Instance().Connect(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT)))
    {
        SetResult(true);
        close();
    }
    else
    {
        LabelAttempts->setText(LANG("ReconnectForm.Attempts").arg(++Attempts));
        Timer->start();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSelectListDialog::ISSelectListDialog(ISNamespace::SelectListMode SelectMode, const QString &TableName, const QString &TableLocalName, int SelectObjectID) : ISInterfaceDialogForm()
{
    setWindowTitle(TableLocalName);
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    QLabel *LabelInfo = new QLabel(this);
    LabelInfo->setFont(FONT_APPLICATION_BOLD);
    ISGui::SetFontWidgetUnderline(LabelInfo, true);
    GetMainLayout()->addWidget(LabelInfo);

    SelectListForm = new ISSelectListForm(SelectMode, TableName, this);
    connect(SelectListForm, &ISSelectListForm::Select, this, &ISSelectListDialog::Selected);
    GetMainLayout()->addWidget(SelectListForm);

    if (SelectObjectID > 0)
    {
        SelectListForm->SetSelectObjectAfterUpdate(SelectObjectID);
    }

    LabelInfo->setText(SelectMode == ISNamespace::SelectListMode::Single ? LANG("SelectDialog.Title.Single") : LANG("SelectDialog.Title.Multi") + ':');
}
//-----------------------------------------------------------------------------
ISSelectListDialog::~ISSelectListDialog()
{

}
//-----------------------------------------------------------------------------
bool ISSelectListDialog::Exec()
{
    SelectListForm->LoadData();
    return ISInterfaceDialogForm::Exec();
}
//-----------------------------------------------------------------------------
ISObjectPair ISSelectListDialog::GetSelectedObject() const
{
    return SelectListForm->GetObject();
}
//-----------------------------------------------------------------------------
ISVectorUInt ISSelectListDialog::GetSelectedObjects() const
{
    return SelectListForm->GetSelectedIDs();
}
//-----------------------------------------------------------------------------
void ISSelectListDialog::AfterShowEvent()
{
    ISInterfaceDialogForm::AfterShowEvent();

    //Изменение размера из конструктора поочему-то не работает, пришлось сделать так
    resize(1000, 700);
    ISGui::MoveWidgetToDesktop(this, ISNamespace::MoveWidgetDesktop::Center);
}
//-----------------------------------------------------------------------------
void ISSelectListDialog::Selected()
{
    SetResult(true);
    close();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSettingsDialog::ISSettingsDialog(const QString &SettingGroupUID) : ISInterfaceDialogForm()
{
    setWindowIcon(BUFFER_ICONS("Settings"));
    setWindowTitle(LANG("Settings"));
    setMinimumSize(900, 600);

    Layout = new QHBoxLayout();
    Layout->setContentsMargins(QMargins(10, 10, 10, 10));
    GetMainLayout()->addLayout(Layout);

    ListWidget = new ISListWidget(this);
    ListWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    ListWidget->setCursor(CURSOR_POINTING_HAND);
    ListWidget->setTabKeyNavigation(false);
    ListWidget->setTabKeyNavigation(false);
    connect(ListWidget, &QListWidget::itemSelectionChanged, this, &ISSettingsDialog::ItemSelectionChanged);
    Layout->addWidget(ListWidget);

    CreateCentralPanel();
    CreateSettings();

    GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

    QHBoxLayout *LayoutBottom = new QHBoxLayout();
    LayoutBottom->setContentsMargins(QMargins(10, 10, 10, 10));
    GetMainLayout()->addLayout(LayoutBottom);

    ISPushButton *ButtonDefault = new ISPushButton(this);
    ButtonDefault->setText(LANG("SettingsDefault"));
    ButtonDefault->setIcon(BUFFER_ICONS("SettingsDefault"));
    connect(ButtonDefault, &ISPushButton::clicked, this, &ISSettingsDialog::DefaultSettings);
    LayoutBottom->addWidget(ButtonDefault);

    LayoutBottom->addStretch();

    ButtonDialog = new ISButtonDialog(this);
    ButtonDialog->SetApplyEnabled(false);
    ButtonDialog->layout()->setContentsMargins(QMargins());
    connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISSettingsDialog::Save);
    connect(ButtonDialog, &ISButtonDialog::Close, this, &ISSettingsDialog::close);
    LayoutBottom->addWidget(ButtonDialog, 0, Qt::AlignRight);

    if (!SettingGroupUID.isEmpty())
    {
        for (int i = 0; i < ListWidget->count(); ++i)
        {
            if (ListWidget->item(i)->data(Qt::UserRole).toString() == SettingGroupUID)
            {
                ListWidget->setCurrentRow(i);
                break;
            }
        }
    }
}
//-----------------------------------------------------------------------------
ISSettingsDialog::~ISSettingsDialog()
{

}
//-----------------------------------------------------------------------------
void ISSettingsDialog::CreateCentralPanel()
{
    LayoutCentral = new QVBoxLayout();
    Layout->addLayout(LayoutCentral);

    QVBoxLayout *LayoutTitle = new QVBoxLayout();

    QWidget *WidgetTitle = new QWidget(this);
    WidgetTitle->setLayout(LayoutTitle);
    LayoutCentral->addWidget(WidgetTitle);

    LabelCurrentGroup = new QLabel(WidgetTitle);
    LabelCurrentGroup->setFont(FONT_TAHOMA_12_BOLD);
    LayoutTitle->addWidget(LabelCurrentGroup, 0, Qt::AlignLeft);

    LabelCurrentGroupHint = new QLabel(WidgetTitle);
    LabelCurrentGroupHint->setVisible(false);
    LabelCurrentGroupHint->setCursor(CURSOR_WHATS_THIS);
    LayoutTitle->addWidget(LabelCurrentGroupHint, 0, Qt::AlignLeft);

    LayoutCentral->addWidget(ISControls::CreateHorizontalLine(this));

    TabWidget = new QTabWidget(this);
    TabWidget->setDocumentMode(true);
    TabWidget->tabBar()->setVisible(false);
    LayoutCentral->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
void ISSettingsDialog::CreateSettings()
{
    for (ISMetaSettingsGroup *MetaGroup : ISSettings::Instance().GetSettingGroups())
    {
        QListWidgetItem* ListWidgetItem = CreateItemGroup(MetaGroup);

        QFormLayout *FormLayout = new QFormLayout();

        ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
        ScrollArea->widget()->setLayout(FormLayout);
        TabWidget->addTab(ScrollArea, QString());

        for (ISMetaSetting *MetaSetting : MetaGroup->Settings)
        {
            QLabel *LabelRow = new QLabel(MetaSetting->LocalName + ':', ScrollArea);
            if (!MetaSetting->Hint.isEmpty())
            {
                LabelRow->setToolTip(ISGui::PrepareLongToolTip(MetaSetting->Hint));
                LabelRow->setCursor(CURSOR_WHATS_THIS);
            }

            ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(ScrollArea, MetaSetting->Type, MetaSetting->WidgetEditName);
            FieldEditBase->SetValue(SETTING_VALUE(MetaSetting->UID));
            FieldEditBase->SetModificationFlag(false);
            connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSettingsDialog::DataChanged);
            FormLayout->addRow(LabelRow, FieldEditBase);
            Fields.emplace(MetaSetting->UID, FieldEditBase);
        }
        Groups.emplace(ListWidgetItem, MetaGroup);
    }

    if (ListWidget->count())
    {
        ListWidget->setCurrentItem(ListWidget->BeginItem());
    }
}
//-----------------------------------------------------------------------------
void ISSettingsDialog::Save()
{
    for (const auto &MapItem : Fields) //Обходим все поля настроек
    {
        if (MapItem.second->GetModificationFlag()) //Если значение поля было изменено - сохраняем его
        {
            ISSettings::Instance().SetValue(MapItem.first, MapItem.second->GetValue());
        }
    }
    close();
}
//-----------------------------------------------------------------------------
void ISSettingsDialog::DefaultSettings()
{
    if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ChangeSettingsToDefault"), LANG("ThisActionCanNotUndone")))
    {
        ISTcpQuery qUserSettingsReset(API_USER_SETTINGS_RESET);
        if (qUserSettingsReset.Execute())
        {
            QVariantMap ResultMap = qUserSettingsReset.GetAnswer()["Result"].toMap();
            for (const auto &MapItem : ResultMap.toStdMap())
            {
                ISSettings::Instance().SetValue(MapItem.first, MapItem.second);
            }
            ISMessageBox::ShowInformation(this, LANG("Message.Information.AppliocationWillBeRestart"));
            close();
        }
        else
        {
            ISMessageBox::ShowCritical(this, qUserSettingsReset.GetErrorString());
        }
    }
}
//-----------------------------------------------------------------------------
QListWidgetItem* ISSettingsDialog::CreateItemGroup(ISMetaSettingsGroup *MetaGroup)
{
    QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
    ListWidgetItem->setText(MetaGroup->LocalName);
    ListWidgetItem->setToolTip(MetaGroup->Hint);
    ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
    ListWidgetItem->setData(Qt::UserRole, MetaGroup->UID);
    if (MetaGroup->System)
    {
        ListWidgetItem->setIcon(BUFFER_ICONS(MetaGroup->IconName));
    }
    return ListWidgetItem;
}
//-----------------------------------------------------------------------------
void ISSettingsDialog::ItemSelectionChanged()
{
    ListWidget->SetFontItems(FONT_TAHOMA_9);
    QListWidgetItem *ClickedItem = ListWidget->currentItem();
    ClickedItem->setFont(FONT_TAHOMA_9_BOLD);
    TabWidget->setCurrentIndex(ListWidget->row(ClickedItem));
    LabelCurrentGroup->setText(ClickedItem->text());

    QString GroupHint = Groups[ClickedItem]->Hint;
    LabelCurrentGroupHint->setText(GroupHint.isEmpty() ? QString() : SYMBOL_STAR + SYMBOL_SPACE + GroupHint);
    LabelCurrentGroupHint->setVisible(!GroupHint.isEmpty());
}
//-----------------------------------------------------------------------------
void ISSettingsDialog::DataChanged()
{
    ButtonDialog->SetApplyEnabled(true);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISUserGroupRightDialog::ISUserGroupRightDialog(int group_id, const QString &group_name)
    : ISInterfaceDialogForm(),
    GroupID(group_id)
{
    setMinimumSize(800, 750);
    setWindowTitle(LANG("AccessRights.SettingAcceessFromGroup").arg(group_name));
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    TabWidget = new QTabWidget(this);
    GetMainLayout()->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
ISUserGroupRightDialog::~ISUserGroupRightDialog()
{

}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::AfterShowEvent()
{
    ISInterfaceDialogForm::AfterShowEvent();
    ISGui::SetWaitGlobalCursor(true);

    ISTcpQuery qGetGroupRights(API_GET_GROUP_RIGHTS);
    qGetGroupRights.BindValue("GroupID", GroupID);
    if (qGetGroupRights.Execute())
    {
        QVariantMap ResultMap = qGetGroupRights.TakeAnswer();
        CreateSubSystems(ResultMap["Systems"].toList());
        CreateTables(ResultMap["RightsTableType"].toList(), ResultMap["Tables"].toList());
        CreateSpecial(ResultMap["Special"].toList());
        ISGui::SetWaitGlobalCursor(false);
    }
    else
    {
        ISGui::SetWaitGlobalCursor(false);
        ISMessageBox::ShowCritical(this, qGetGroupRights.GetErrorString());
        close();
    }
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::CreateSubSystems(const QVariantList &Systems)
{
    ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
    ScrollArea->widget()->setLayout(new QVBoxLayout());
    TabWidget->addTab(ScrollArea, LANG("AccessRights.SubSystems"));

    for (const QVariant &VariantSystem : Systems) //Обход всех систем
    {
        QVariantMap SystemMap = VariantSystem.toMap();

        QGroupBox *GroupBox = new QGroupBox(SystemMap["LocalName"].toString(), ScrollArea);
        GroupBox->setLayout(new QVBoxLayout());
        GroupBox->setStyleSheet(BUFFER_STYLE_SHEET("QGroupBoxAccessSubSystem"));
        ScrollArea->widget()->layout()->addWidget(GroupBox);

        for (const QVariant &VariantSubSystem : SystemMap["SubSystems"].toList()) //Обход всех подсистем текущей системы
        {
            QVariantMap SubSystemMap = VariantSubSystem.toMap();

            ISCheckEdit *CheckEdit = new ISCheckEdit(GroupBox);
            CheckEdit->SetValue(SubSystemMap["IsExist"]);
            CheckEdit->SetText(SubSystemMap["LocalName"].toString());
            CheckEdit->setProperty("SubSystemUID", SubSystemMap["UID"]);
            CheckEdit->SetToolTip(LANG("AccessRights.ClickedToGiveAccessFromSubSystem"));
            connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupRightDialog::SubSystemClicked);
            GroupBox->layout()->addWidget(CheckEdit);
        }
    }
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::CreateTables(const QVariantList &RightTableType, const QVariantList &Tables)
{
    QFormLayout *FormLayout = new QFormLayout();
    ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
    ScrollArea->widget()->setLayout(FormLayout);
    TabWidget->addTab(ScrollArea, LANG("AccessRights.Tables"));

    for (const QVariant &VariantTable : Tables)
    {
        QVariantMap TableMap = VariantTable.toMap();

        QToolBar *ToolBar = new QToolBar(ScrollArea);
        ToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ToolBar->setIconSize(QSize(20, 20));
        connect(ToolBar, &QToolBar::actionTriggered, this, &ISUserGroupRightDialog::TableClicked);
        FormLayout->addRow(TableMap["LocalName"].toString() + ':', ToolBar);

        for (const QVariant &VariantRight : RightTableType) //Обходим типы прав
        {
            QVariantMap RightMap = VariantRight.toMap();

            QAction *Action = ToolBar->addAction(BUFFER_ICONS(RightMap["Icon"].toString()), RightMap["Name"].toString());
            Action->setCheckable(true);
            Action->setProperty("TableName", TableMap["TableName"]);
            Action->setProperty("AccessUID", RightMap["UID"]);
            Action->setChecked(TableMap["Rights"].toList().contains(RightMap["UID"]));
            ToolBar->widgetForAction(Action)->setCursor(CURSOR_POINTING_HAND);
        }
    }
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::CreateSpecial(const QVariantList &SpecialRights)
{
    ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
    ScrollArea->widget()->setLayout(new QVBoxLayout());
    TabWidget->addTab(ScrollArea, LANG("AccessRights.SpecialRoles"));

    //Обходим группы спец. прав
    for (const QVariant &SpecialRightGroup : SpecialRights)
    {
        QVariantMap SpecialRightGroupMap = SpecialRightGroup.toMap();

        QGroupBox *GroupBox = new QGroupBox(SpecialRightGroupMap["LocalName"].toString() + ':', ScrollArea);
        GroupBox->setLayout(new QVBoxLayout());
        GroupBox->setStyleSheet(BUFFER_STYLE_SHEET("QGroupBoxAccessSubSystem"));
        ScrollArea->widget()->layout()->addWidget(GroupBox);

        //Обходим спец. права текущей группы
        for (const QVariant &SpecialRight : SpecialRightGroupMap["Rights"].toList())
        {
            QVariantMap SpecialRightMap = SpecialRight.toMap();

            ISCheckEdit *CheckEdit = new ISCheckEdit(GroupBox);
            CheckEdit->SetValue(SpecialRightMap["IsExist"]);
            CheckEdit->SetText(SpecialRightMap["LocalName"].toString());
            CheckEdit->CreateHint(SpecialRightMap["Hint"].toString());
            CheckEdit->setProperty("UID", SpecialRightMap["UID"]);
            connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISUserGroupRightDialog::SpecialClicked);
            GroupBox->layout()->addWidget(CheckEdit);
        }
        dynamic_cast<QVBoxLayout*>(GroupBox->layout())->addStretch();
    }
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::SubSystemClicked(const QVariant &value)
{
    ISGui::SetWaitGlobalCursor(true);
    ISTcpQuery qAlterAccess(value.toBool() ? API_GROUP_RIGHT_SUBSYSTEM_ADD : API_GROUP_RIGHT_SUBSYSTEM_DELETE);
    qAlterAccess.BindValue("GroupID", GroupID);
    qAlterAccess.BindValue("UID", sender()->property("SubSystemUID"));
    if (!qAlterAccess.Execute())
    {
        ISGui::SetWaitGlobalCursor(false);
        ISMessageBox::ShowCritical(this, qAlterAccess.GetErrorString());
    }
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::TableClicked(QAction *Action)
{
    ISGui::SetWaitGlobalCursor(true);
    ISTcpQuery qAlterAccess(Action->isChecked() ? API_GROUP_RIGHT_TABLE_ADD : API_GROUP_RIGHT_TABLE_DELETE);
    qAlterAccess.BindValue("GroupID", GroupID);
    qAlterAccess.BindValue("TableName", Action->property("TableName"));
    qAlterAccess.BindValue("AccessUID", Action->property("AccessUID"));
    if (!qAlterAccess.Execute())
    {
        ISGui::SetWaitGlobalCursor(false);
        ISMessageBox::ShowCritical(this, qAlterAccess.GetErrorString());
    }
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserGroupRightDialog::SpecialClicked(const QVariant &value)
{
    ISGui::SetWaitGlobalCursor(true);
    ISTcpQuery qGroupRightSpecial(value.toBool() ? API_GROUP_RIGHT_SPECIAL_ADD : API_GROUP_RIGHT_SPECIAL_DELETE);
    qGroupRightSpecial.BindValue("GroupID", GroupID);
    qGroupRightSpecial.BindValue("SpecialRightUID", sender()->property("UID"));
    if (!qGroupRightSpecial.Execute())
    {
        ISGui::SetWaitGlobalCursor(false);
        ISMessageBox::ShowCritical(this, qGroupRightSpecial.GetErrorString());
    }
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISUserPasswordDialog::ISUserPasswordDialog(unsigned int user_id, const QString &user_fio, const QString &user_login)
    : ISInterfaceDialogForm(),
    EditPasswordCurrent(nullptr),
    UserID(user_id),
    UserFIO(user_fio),
    UserLogin(user_login)
{
    ISTcpQuery qUserPasswordExist(API_USER_PASSWORD_EXIST);
    qUserPasswordExist.BindValue("UserID", UserID);
    if (!qUserPasswordExist.Execute())
    {
        ISMessageBox::ShowWarning(this, qUserPasswordExist.GetErrorString());
        return;
    }
    PasswordExist = qUserPasswordExist.GetAnswer()["IsExist"].toBool();

    setWindowTitle(PasswordExist ? LANG("UserPasswordForm.Title.Change") : LANG("UserPasswordForm.Title.Creating"));
    ForbidResize();
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    QLabel *LabelUser = new QLabel(LANG("User") + ": " + UserFIO, this);
    LabelUser->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
    ISGui::SetFontWidgetBold(LabelUser, true);
    GetMainLayout()->addWidget(LabelUser);

    GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

    if (PasswordExist) //Если пароль существует - добавляем поле с текущим паролем для изменения
    {
        //Текущий пароль
        GetMainLayout()->addWidget(new QLabel(LANG("UserPasswordForm.CurrentPassword"), this), 0, Qt::AlignLeft);
        EditPasswordCurrent = new ISLineEdit(this);
        EditPasswordCurrent->SetEchoMode(QLineEdit::Password);
        GetMainLayout()->addWidget(EditPasswordCurrent);
    }

    GetMainLayout()->addWidget(new QLabel(LANG("UserPasswordForm.Password"), this), 0, Qt::AlignLeft);
    EditPassword = new ISLineEdit(this);
    EditPassword->SetEchoMode(QLineEdit::Password);
    GetMainLayout()->addWidget(EditPassword);

    GetMainLayout()->addWidget(new QLabel(LANG("UserPasswordForm.PasswordCheck"), this), 0, Qt::AlignLeft);
    EditPasswordCheck = new ISLineEdit(this);
    EditPasswordCheck->SetEchoMode(QLineEdit::Password);
    GetMainLayout()->addWidget(EditPasswordCheck);

    ButtonDialog = new ISButtonDialog(this);
    connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISUserPasswordDialog::Apply);
    connect(ButtonDialog, &ISButtonDialog::Close, this, &ISUserPasswordDialog::close);
    GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISUserPasswordDialog::~ISUserPasswordDialog()
{

}
//-----------------------------------------------------------------------------
void ISUserPasswordDialog::AfterShowEvent()
{
    ISInterfaceDialogForm::AfterShowEvent();
    PasswordExist ? EditPasswordCurrent->SetFocus() : EditPassword->SetFocus();
}
//-----------------------------------------------------------------------------
void ISUserPasswordDialog::EnterClicked()
{
    Apply();
}
//-----------------------------------------------------------------------------
void ISUserPasswordDialog::Apply()
{
    QString PasswordCurrent, Password, PasswordCheck;
    if (PasswordExist)
    {
        PasswordCurrent = EditPasswordCurrent->GetValue().toString();
        Password = EditPassword->GetValue().toString();
        PasswordCheck = EditPasswordCheck->GetValue().toString();
        if (PasswordCurrent.isEmpty() || Password.isEmpty() || PasswordCheck.isEmpty())
        {
            ISMessageBox::ShowWarning(this, LANG("Message.Warning.OneFieldNotFill"));
            return;
        }
    }
    else
    {
        Password = EditPassword->GetValue().toString();
        PasswordCheck = EditPasswordCheck->GetValue().toString();
        if (Password.isEmpty() || PasswordCheck.isEmpty())
        {
            ISMessageBox::ShowWarning(this, LANG("Message.Warning.OneFieldNotFill"));
            return;
        }
    }

    //Проверка сложности пароля
    if (!ISAlgorithmOld::PasswordVerification(Password))
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.PasswordVerification"));
        EditPassword->BlinkRed();
        return;
    }

    if (Password != PasswordCheck)
    {
        ISMessageBox::ShowCritical(this, LANG("Message.Error.PasswordsNotEqual"));
        return;
    }

    ISTcpQuery qUserPassword(PasswordExist ? API_USER_PASSWORD_EDIT : API_USER_PASSWORD_CREATE);
    qUserPassword.BindValue("UserID", UserID);
    if (PasswordExist)
    {
        qUserPassword.BindValue("HashOld", ISAlgorithm::StringToSHA256(UserLogin.toStdString() + PasswordCurrent.toStdString()).c_str());
    }
    qUserPassword.BindValue("Hash", ISAlgorithm::StringToSHA256(UserLogin.toStdString() + Password.toStdString()).c_str());
    if (qUserPassword.Execute())
    {
        ISPopupMessage::ShowNotification(PasswordExist ? LANG("UserPasswordForm.EditedPassword") : LANG("UserPasswordForm.CreatedPassword"));
        SetResult(true);
        close();
    }
    else
    {
        ISMessageBox::ShowWarning(this, qUserPassword.GetErrorString());
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISRecordInfoDialog::ISRecordInfoDialog(QWidget *parent, const QString &TableName, unsigned int ObjectID) : ISInterfaceDialogForm(parent)
{
    setWindowTitle(LANG("ISRecordInfoDialog.Title"));
    setWindowIcon(BUFFER_ICONS("RecordInfo"));
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    QFormLayout *FormLayout = new QFormLayout();
    GetMainLayout()->addLayout(FormLayout);

    ISTcpQuery qRecordGetInfo(API_RECORD_GET_INFO);
    qRecordGetInfo.BindValue("TableName", TableName);
    qRecordGetInfo.BindValue("ObjectID", ObjectID);
    if (qRecordGetInfo.Execute())
    {
        QVariantMap AnswerMap = qRecordGetInfo.GetAnswer();

        QLabel *LabelTableName = new QLabel(LANG("ISRecordInfoDialog.Field.TableName"), this);
        ISGui::SetFontWidgetBold(LabelTableName, true);
        FormLayout->addRow(LabelTableName, new QLabel(AnswerMap["TableName"].toString(), this));

        QLabel *LabelName = new QLabel(LANG("ISRecordInfoDialog.Field.Name"), this);
        ISGui::SetFontWidgetBold(LabelName, true);
        FormLayout->addRow(LabelName, new QLabel(AnswerMap["ObjectName"].toString(), this));

        FormLayout->addWidget(ISControls::CreateHorizontalLine(this));

        QLabel *LabelCreateDate = new QLabel(LANG("ISRecordInfoDialog.Field.CreateDate"), this);
        ISGui::SetFontWidgetBold(LabelCreateDate, true);
        FormLayout->addRow(LabelCreateDate, new QLabel(AnswerMap["CreateDate"].toString(), this));

        QLabel *LabelCreateUser = new QLabel(LANG("ISRecordInfoDialog.Field.CreateUser"), this);
        ISGui::SetFontWidgetBold(LabelCreateUser, true);
        FormLayout->addRow(LabelCreateUser, new QLabel(AnswerMap["CreateUser"].toString(), this));

        QLabel *LabelEditDate = new QLabel(LANG("ISRecordInfoDialog.Field.EditDate"), this);
        ISGui::SetFontWidgetBold(LabelEditDate, true);
        FormLayout->addRow(LabelEditDate, new QLabel(AnswerMap["EditDate"].toString(), this));

        QLabel *LabelEditUser = new QLabel(LANG("ISRecordInfoDialog.Field.EditUser"), this);
        ISGui::SetFontWidgetBold(LabelEditUser, true);
        FormLayout->addRow(LabelEditUser, new QLabel(AnswerMap["EditUser"].toString(), this));

        QLabel *LabelCopyCount = new QLabel(LANG("ISRecordInfoDialog.Field.CopyCount"), this);
        ISGui::SetFontWidgetBold(LabelCopyCount, true);
        FormLayout->addRow(LabelCopyCount, new QLabel(QString::number(AnswerMap["CopyCount"].toInt()), this));

        QLabel *LabelEditCount = new QLabel(LANG("ISRecordInfoDialog.Field.EditCount"), this);
        ISGui::SetFontWidgetBold(LabelEditCount, true);
        FormLayout->addRow(LabelEditCount, new QLabel(QString::number(AnswerMap["EditCount"].toInt()), this));

        QLabel *LabelShowCount = new QLabel(LANG("ISRecordInfoDialog.Field.ShowCount"), this);
        ISGui::SetFontWidgetBold(LabelShowCount, true);
        FormLayout->addRow(LabelShowCount, new QLabel(QString::number(AnswerMap["ShowCount"].toInt()), this));

        QLabel *LabelFavoriteCount = new QLabel(LANG("ISRecordInfoDialog.Field.FavoriteCount"), this);
        ISGui::SetFontWidgetBold(LabelFavoriteCount, true);
        FormLayout->addRow(LabelFavoriteCount, new QLabel(QString::number(AnswerMap["FavoriteCount"].toInt()), this));

        QLabel *LabelProtocolCount = new QLabel(LANG("ISRecordInfoDialog.Field.ProtocolCount"), this);
        ISGui::SetFontWidgetBold(LabelProtocolCount, true);
        FormLayout->addRow(LabelProtocolCount, new QLabel(QString::number(AnswerMap["ProtocolCount"].toInt()), this));

        QLabel *LabelDiscussionCount = new QLabel(LANG("ISRecordInfoDialog.Field.DiscussionCount"), this);
        ISGui::SetFontWidgetBold(LabelDiscussionCount, true);
        FormLayout->addRow(LabelDiscussionCount, new QLabel(QString::number(AnswerMap["DiscussionCount"].toInt()), this));
    }
    else
    {
        ISMessageBox::ShowCritical(this, qRecordGetInfo.GetErrorString());
    }

    ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
    connect(ButtonClose, &ISPushButton::clicked, this, &ISRecordInfoDialog::close);
    GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISRecordInfoDialog::~ISRecordInfoDialog()
{

}
//-----------------------------------------------------------------------------
