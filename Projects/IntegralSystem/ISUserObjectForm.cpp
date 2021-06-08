#include "ISUserObjectForm.h"
#include "ISControls.h"
#include "ISLocalization.h"
#include "ISDialogsCommon.h"
#include "ISGui.h"
#include "ISTcpQuery.h"
#include "ISBuffer.h"
#include "ISPopupMessage.h"
#include "ISDialogsForm.h"
//-----------------------------------------------------------------------------
ISUserObjectForm::ISUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
    EditAccountLifeTime = GetFieldWidget("AccountLifeTime");
    connect(EditAccountLifeTime, &ISFieldEditBase::DataChanged, this, &ISUserObjectForm::AccountLifeTimeChanged);

    EditAccountLifeTimeStart = GetFieldWidget("AccountLifeTimeStart");
    EditAccountLifeTimeEnd = GetFieldWidget("AccountLifeTimeEnd");

    QAction *ActionPassword = new QAction(BUFFER_ICONS("User.Password"), LANG("PasswordManagement"), this);
    connect(ActionPassword, &QAction::triggered, this, &ISUserObjectForm::PasswordManagement);
    AddActionToolBar(ActionPassword, true);

    QAction *ActionPasswordReset = new QAction(BUFFER_ICONS("User.Password.Reset"), LANG("PasswordReset"), this);
    connect(ActionPasswordReset, &QAction::triggered, this, &ISUserObjectForm::PasswordReset);
    AddActionToolBar(ActionPasswordReset, true);
}
//-----------------------------------------------------------------------------
ISUserObjectForm::~ISUserObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISUserObjectForm::Save()
{
    //Проверка привязки учётной записи к группе
    bool Result = GetFieldValue("Group").isValid();
    if (!Result)
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.UserNotLinkedToGroup"));
        GetFieldWidget("Group")->BlinkRed();
        return Result;
    }

    //Проверка корректности ввода диапазона срока действия учётной записи
    if (EditAccountLifeTime->GetValue().toBool())
    {
        QDate DateStart = EditAccountLifeTimeStart->GetValue().toDate(),
            DateEnd = EditAccountLifeTimeEnd->GetValue().toDate();
        if (DateStart > DateEnd || DateEnd < DateStart || !DateStart.isValid() || !DateEnd.isValid())
        {
            ISMessageBox::ShowWarning(this, LANG("Message.Warning.InvalidAccountLifeTimeDatesUser"));
            EditAccountLifeTimeStart->BlinkRed();
            EditAccountLifeTimeEnd->BlinkRed();
            return false;
        }
    }

    if (GetFormType() == ISNamespace::ObjectFormType::New || GetFormType() == ISNamespace::ObjectFormType::Copy)
    {
        Result = ISObjectFormBase::Save();
        if (Result)
        {
            bool PasswordCreated = false;
            ISGui::ShowUserPasswordForm(GetObjectID(), GetFieldValue("FIO").toString(), GetFieldValue("Login").toString(), PasswordCreated);
            if (!PasswordCreated)
            {
                ISMessageBox::ShowWarning(this, LANG("Message.Warning.UserPasswordNotCreated"));
            }
        }
    }
    else
    {
        Result = ISObjectFormBase::Save();
    }
    return Result;
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::PasswordManagement()
{
    if (GetModificationFlag())
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.SaveObjectFromContinue"));
        return;
    }
    ISGui::ShowUserPasswordForm(GetObjectID(), GetFieldValue("FIO").toString(), GetFieldValue("Login").toString());
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::PasswordReset()
{
    if (GetModificationFlag())
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.SaveObjectFromContinue"));
        return;
    }

    if (ISMessageBox::ShowQuestion(nullptr, LANG("Message.Question.PasswordReset"), LANG("ThisActionIsNotReversible")))
    {
        ISTcpQuery qPasswordReset(API_USER_PASSWORD_RESET);
        qPasswordReset.BindValue("UserID", GetObjectID());
        if (qPasswordReset.Execute())
        {
            ISPopupMessage::ShowNotification(LANG("PasswordResetDone"));
        }
        else
        {
            ISMessageBox::ShowCritical(nullptr, qPasswordReset.GetErrorString());
        }
    }
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::AccountLifeTimeChanged()
{
    if (!EditAccountLifeTime->GetValue().toBool())
    {
        EditAccountLifeTimeStart->Clear();
        EditAccountLifeTimeEnd->Clear();
    }
}
//-----------------------------------------------------------------------------
