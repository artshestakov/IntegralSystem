#include "ISMainWindow.h"
#include "ISGui.h"
#include "ISLocalizationOld.h"
#include "ISBuffer.h"
#include "ISDialogsCommon.h"
#include "ISControls.h"
#include "ISUserRoleEntity.h"
#include "ISAssert.h"
#include "ISConstantsOld.h"
#include "ISParagraphEntity.h"
#include "ISPopupMessage.h"
#include "ISCreatedObjectsEntity.h"
#include "ISObjects.h"
#include "ISProperty.h"
#include "ISConfig.h"
#include "ISTcpConnector.h"
#include "ISDialogsForm.h"
#include "ISPopupMessage.h"
#include "ISTcpQuery.h"
#include "ISConstantsGui.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
ISMainWindow::ISMainWindow(QWidget *parent)
    : ISInterfaceForm(parent),
    ExitConfirm(true),
    ExistCheckModifie(true),
    PropertyAnimation(nullptr)
{
    connect(&ISCreatedObjectsEntity::Instance(), &ISCreatedObjectsEntity::Existed, this, &ISMainWindow::ActivateWorkspace);

    ISBuffer::Instance().ConfigurationInfo.LocalName.isEmpty() ?
        setWindowTitle("IntegralSystem: " + ISBuffer::Instance().CurrentUserInfo.FIO) :
        setWindowTitle("IntegralSystem - " + ISBuffer::Instance().ConfigurationInfo.LocalName + " : " + ISBuffer::Instance().CurrentUserInfo.FIO);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setWindowIcon(BUFFER_ICONS("Logo"));
    resize(QSize(800, 559));
    setMinimumSize(SIZE_MAIN_WINDOW_MINIMUM);
    GetMainLayout()->setSpacing(0);

    MenuBar = new ISMenuBar(this);
    connect(MenuBar, static_cast<void(ISMenuBar::*)(const QString &)>(&ISMenuBar::ParagraphClicked), this, &ISMainWindow::ParagraphClicked);
    connect(MenuBar, &ISMenuBar::ChangeUser, this, &ISMainWindow::ChangeUser);
    connect(MenuBar, &ISMenuBar::Exit, this, &ISMainWindow::close);
    connect(MenuBar, &ISMenuBar::Favorites, this, &ISMainWindow::ShowFavoritesForm);
    connect(MenuBar, &ISMenuBar::History, this, &ISMainWindow::ShowHistoryForm);
    connect(MenuBar, &ISMenuBar::DatabaseSettings, this, &ISMainWindow::ShowDatabaseSettings);
    connect(MenuBar, &ISMenuBar::ChangePassword, this, &ISMainWindow::ShowChangePasswordForm);
    connect(MenuBar, &ISMenuBar::Settings, this, &ISMainWindow::ShowSettingsForm);
    connect(MenuBar, &ISMenuBar::AboutApplication, this, &ISMainWindow::ShowAboutForm);
    GetMainLayout()->addWidget(MenuBar);

    StackedWidget = new QStackedWidget(this);
    GetMainLayout()->addWidget(StackedWidget);
    for (ISMetaParagraph *MetaParagraph : ISParagraphEntity::Instance().GetParagraphs())
    {
        Paragraphs[MetaParagraph->UID] = StackedWidget->addWidget(ISGui::CreatePointer<ISParagraphBaseForm *>(MetaParagraph->ClassName, Q_ARG(QWidget *, this)));
    }

    connect(&ISTcpConnector::Instance(), &ISTcpConnector::Reconnect, this, &ISMainWindow::Reconnect);
}
//-----------------------------------------------------------------------------
ISMainWindow::~ISMainWindow()
{

}
//-----------------------------------------------------------------------------
void ISMainWindow::closeEvent(QCloseEvent *CloseEvent)
{
    if (ExistCheckModifie) //Проверять наличие несохраненных записей
    {
        if (!ISCreatedObjectsEntity::Instance().CheckExistForms())
        {
            CloseEvent->ignore();
        }
    }

    bool Answer = true;
    if (ExitConfirm) //Требуется подтверждение выхода
    {
        SetVisibleShadow(true);
        Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ExitApplication"));
        SetVisibleShadow(false);
    }
    //При подтверждении выхода обязательно вызывать quit(), иначе основной поток событий будет висеть и программа не закроется
    Answer ? qApp->quit() : CloseEvent->ignore();
}
//-----------------------------------------------------------------------------
void ISMainWindow::AfterShowEvent()
{
    ISInterfaceForm::AfterShowEvent();
    QTimer::singleShot(3000, ISObjects::Instance().GetInterface(), &ISObjectInterface::InitializePlugin);
}
//-----------------------------------------------------------------------------
void ISMainWindow::EscapeClicked()
{
    close();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ParagraphClicked(const QString &ParagraphUID)
{
    if (CurrentParagraphUID == ParagraphUID)
    {
        return;
    }
    ISGui::SetWaitGlobalCursor(true);
    ISParagraphBaseForm *ParagraphWidget = dynamic_cast<ISParagraphBaseForm*>(StackedWidget->widget(Paragraphs[ParagraphUID]));
    StackedWidget->setCurrentWidget(ParagraphWidget);
    ParagraphWidget->Invoke();
    ParagraphWidget->setFocus();
    CurrentParagraphUID = ParagraphUID;
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::RollUp()
{
    PropertyAnimation->start();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ChangeUser()
{
    ExitConfirm = false;
    SetVisibleShadow(true);
    bool Change = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ChangeUser"));
    SetVisibleShadow(false);
    if (Change)
    {
        ExitConfirm = false;
        ISProperty::Instance().SetChangeUser(true);
        close();
    }
    else
    {
        ExitConfirm = true;
    }
}
//-----------------------------------------------------------------------------
void ISMainWindow::ActivateWorkspace()
{
    ParagraphClicked(CONST_UID_PARAGRAPH_WORKSPACE);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowFavoritesForm()
{
    ISGui::ShowFavoritesForm();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowHistoryForm()
{
    ISGui::ShowHistoryForm();
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowDatabaseSettings()
{
    ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, "_SettingsDatabase", SETTING_DATABASE_VALUE_INT("ID")));
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowChangePasswordForm()
{
    if (ISBuffer::Instance().CurrentUserInfo.System) //Если пользователь системный - не разрешаем менять пароль
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotChangeSystemUserPassword"));
        return;
    }
    SetVisibleShadow(true);
    ISGui::ShowUserPasswordForm(CURRENT_USER_ID, ISBuffer::Instance().CurrentUserInfo.FIO, ISBuffer::Instance().CurrentUserInfo.Login);
    SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowSettingsForm()
{
    SetVisibleShadow(true);
    ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_GENERAL);
    SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::ShowAboutForm()
{
    SetVisibleShadow(true);
    ISAboutDialog().Exec();
    SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
void ISMainWindow::Reconnect()
{
    SetVisibleShadow(true);
    ISReconnectDialog ReconnectForm;
    bool Result = ReconnectForm.Exec();
    if (Result) //Переподключение прошло успешно - посылаем запрос на авторизацию
    {
        ISGui::SetWaitGlobalCursor(false);

        ISTcpQuery qReAuth(API_AUTH);
        qReAuth.BindValue("Hash", ISAlgorithm::StringToSHA256(ISBuffer::Instance().CurrentUserInfo.Login.toStdString() + ISBuffer::Instance().CurrentUserInfo.Password.toStdString()).c_str());
        Result = qReAuth.Execute();
        if (Result)
        {
            ISPopupMessage::ShowNotification(LANG("ReconnectingDone"));

            //Проверяем, не повысилась ли версия сервера
            if (qReAuth.GetAnswer()["Server"].toMap()["Version"].toUInt() > ISProperty::Instance().GetServerVersion())
            {
                ISMessageBox::ShowInformation(this, LANG("Message.Information.RestartAfterReconnect"));
            }
        }
        else
        {
            ISMessageBox::ShowCritical(this, qReAuth.GetErrorString());
        }
    }

    if (!Result) //Переподключения не произошло - выходим из программы
    {
        ExitConfirm = false;
        ExistCheckModifie = false;
        close();
    }
    SetVisibleShadow(false);
}
//-----------------------------------------------------------------------------
