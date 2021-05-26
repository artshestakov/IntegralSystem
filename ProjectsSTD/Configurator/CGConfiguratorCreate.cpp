#include "CGConfiguratorCreate.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISDebug.h"
#include "ISMetaData.h"
#include "ISConsole.h"
//#include "ISSystem.h"
#include "CGDatabase.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static std::string QI_SYSTEM_USER = PREPARE_QUERY("INSERT INTO _users(usrs_uid, usrs_issystem, usrs_fio, usrs_login, usrs_accessallowed) "
    "VALUES(:UID, true, :FIO, :Login, true)");
//-----------------------------------------------------------------------------
static std::string QU_SYSTEM_USER_PASSWORD = PREPARE_QUERY("UPDATE _users SET "
    "usrs_hash = :Hash, "
    "usrs_salt = :Salt "
    "WHERE usrs_uid = :UID");
//-----------------------------------------------------------------------------
CGConfiguratorCreate::CGConfiguratorCreate() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorCreate::~CGConfiguratorCreate()
{

}
//-----------------------------------------------------------------------------
/*bool CGConfiguratorCreate::adminaccount()
{
    ISQuery qInsertAccount(QI_SYSTEM_USER);
    qInsertAccount.BindValue(":UID", SYSTEM_USER_UID);
    qInsertAccount.BindValue(":FIO", LANG("Configurator.AdminAccountFIO"));
    qInsertAccount.BindValue(":Login", SYSTEM_USER_LOGIN);
    qInsertAccount.SetShowLongQuery(false);
    bool Result = qInsertAccount.Execute();
    if (Result) //������� ������ ���� ������� ��������� - ���������� ������� ������
    {
        ISDEBUG_L("Admin account created successfully!");
        Result = adminpassword();
    }
    else //������ �������
    {
        if (qInsertAccount.GetErrorNumber() == 23505) //���� ��������� ������������ - ������ ������� ������ ��� �������
        {
            ErrorString = "Admin account already exist";
        }
        else
        {
            ErrorString = qInsertAccount.GetErrorString();
        }
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGConfiguratorCreate::adminpassword()
{
    //������ ������ ������
    QString Password;
    while (true)
    {
        Password = ISConsole::GetString("Create password for system administrator: ");
        if (Password.isEmpty()) //���� ������ �� ��� ����� - ������� � �������
        {
            ISDEBUG_L("Password was not entered!");
            return false;
        }

        if (ISAlgorithm::PasswordVerification(Password))
        {
            ISDEBUG_L("WARNING! Keep password in a safe place.");
            break;
        }
        else
        {
            ISDEBUG_L("Invalid password!");
        }
    }

    //��������� ���, ���������� ���� � ����� ������
    QString Hash = QString::fromStdString(ISAlgorithm::StringToSha256(std::string(SYSTEM_USER_LOGIN) + Password.toStdString())), Salt;
    if (!ISAlgorithm::GenerateSalt(Salt, ErrorString))
    {
        return false;
    }
    QString HashResult = ISAlgorithm::SaltPassword(Hash, Salt);

    ISQuery qUpdatePassword(QU_SYSTEM_USER_PASSWORD);
    qUpdatePassword.BindValue(":Hash", HashResult);
    qUpdatePassword.BindValue(":Salt", Salt);
    qUpdatePassword.BindValue(":UID", SYSTEM_USER_UID);
    qUpdatePassword.SetShowLongQuery(false);
    if (qUpdatePassword.Execute())
    {
        if (qUpdatePassword.GetCountAffected() == 1) //������ ������� ����������
        {
            ISDEBUG_L("Password created successfully");
        }
        else if (qUpdatePassword.GetCountAffected() == 0) //������� ������ �������������� �� ����������
        {
            ErrorString = "The administrator account does not exist";
            return false;
        }
        else //����������� ������
        {
            ErrorString = "Unknown error";
            return false;
        }
    }
    else //������ �������
    {
        ErrorString = qUpdatePassword.GetErrorString();
    }
    return true;
}*/
//-----------------------------------------------------------------------------
/*bool CGConfiguratorCreate::resources()
{
    bool Result = true, Exist = true;
    for (size_t i = 0, CountResources = ISMetaData::Instance().GetResources().size(); i < CountResources; ++i)
    {
        PMetaResource *MetaResource = ISMetaData::Instance().GetResources().at(i);
        Progress("Resources for " + MetaResource->TableName, i, CountResources);

        if (CGDatabase::Resource_Exist(MetaResource, Exist, ErrorString))
        {
            if (!Exist)
            {
                Result = CGDatabase::Resource_Insert(MetaResource, ErrorString);
            }
        }

        if (!Result)
        {
            break;
        }
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
