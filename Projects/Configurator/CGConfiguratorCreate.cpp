#include "CGConfiguratorCreate.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISDebug.h"
#include "ISMetaData.h"
#include "ISConsole.h"
#include "CGDatabase.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
static std::string QI_SYSTEM_USER = PREPARE_QUERY("INSERT INTO _users(usrs_uid, usrs_issystem, usrs_fio, usrs_login, usrs_accessallowed) "
    "VALUES($1, true, $2, $3, true)");
//-----------------------------------------------------------------------------
static std::string QU_SYSTEM_USER_PASSWORD = PREPARE_QUERY("UPDATE _users SET "
    "usrs_hash = $1, "
    "usrs_salt = $2 "
    "WHERE usrs_uid = $3");
//-----------------------------------------------------------------------------
CGConfiguratorCreate::CGConfiguratorCreate() : CGConfiguratorBase()
{
    RegisterFunction("adminaccount", static_cast<Function>(&CGConfiguratorCreate::adminaccount));
    RegisterFunction("adminpassword", static_cast<Function>(&CGConfiguratorCreate::adminpassword));
    RegisterFunction("resources", static_cast<Function>(&CGConfiguratorCreate::resources));
}
//-----------------------------------------------------------------------------
CGConfiguratorCreate::~CGConfiguratorCreate()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorCreate::adminaccount()
{
    ISQuery qInsertAccount(QI_SYSTEM_USER);
    qInsertAccount.BindUID(DB_SYSTEM_USER_UID);
    qInsertAccount.BindString(DB_SYSTEM_USER_FIO);
    qInsertAccount.BindString(DB_SYSTEM_USER_LOGIN);
    qInsertAccount.SetShowLongQuery(false);
    bool Result = qInsertAccount.Execute();
    if (Result) //Учётная запись была успешно добавлена - предлагаем создать пароль
    {
        ISLOGGER_I(__CLASS__, "Admin account created successfully!");
        Result = adminpassword();
    }
    else //Ошибка запроса
    {
        if (qInsertAccount.GetErrorNumber() == 23505) //Если нарушение уникальности - значит учётная запись уже создана
        {
            ErrorString = "Admin account already exist";
        }
        else
        {
            ErrorString = qInsertAccount.GetErrorString();
        }
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorCreate::adminpassword()
{
    //Просим ввести пароль
    std::string Password;
    while (true)
    {
        Password = ISConsole::GetString("Create password for system administrator: ");
        if (Password.empty()) //Если пароль не был введён - выходим с ошибкой
        {
            ISLOGGER_I(__CLASS__, "Password was not entered!");
            return false;
        }

        if (ISAlgorithm::PasswordVerification(Password))
        {
            ISLOGGER_I(__CLASS__, "WARNING! Keep password in a safe place.");
            break;
        }
        else
        {
            ISLOGGER_I(__CLASS__, "Invalid password!");
        }
    }

    //Формируем хэш, генерируем соль и солим пароль
    std::string Hash = ISAlgorithm::StringToSHA256(DB_SYSTEM_USER_LOGIN + Password), Salt;
    if (!ISAlgorithm::GenerateSalt(Salt, ErrorString))
    {
        return false;
    }
    std::string HashResult = ISAlgorithm::SaltPassword(Hash, Salt);

    ISQuery qUpdatePassword(QU_SYSTEM_USER_PASSWORD);
    qUpdatePassword.BindString(HashResult);
    qUpdatePassword.BindString(Salt);
    qUpdatePassword.BindUID(DB_SYSTEM_USER_UID);
    qUpdatePassword.SetShowLongQuery(false);
    if (qUpdatePassword.Execute())
    {
        size_t AffectedRows = qUpdatePassword.GetResultAffected();
        if (AffectedRows == 1) //Пароль успешно установлен
        {
            ISLOGGER_I(__CLASS__, "Password created successfully");
        }
        else if (AffectedRows == 0) //Учётная запись администратора не существует
        {
            ErrorString = "The administrator account does not exist";
            return false;
        }
        else //Неизвестная ошибка
        {
            ErrorString = "Unknown error";
            return false;
        }
    }
    else //Ошибка запроса
    {
        ErrorString = qUpdatePassword.GetErrorString();
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorCreate::resources()
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
}
//-----------------------------------------------------------------------------
