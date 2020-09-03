#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISMetaData.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
static QString QS_DATABASE = PREPARE_QUERY("SELECT COUNT(*) "
                                           "FROM pg_database "
                                           "WHERE datname = :DatabaseName");
//-----------------------------------------------------------------------------
ISDatabase::ISDatabase()
    : ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISDatabase::~ISDatabase()
{

}
//-----------------------------------------------------------------------------
ISDatabase& ISDatabase::Instance()
{
    static ISDatabase Database;
    return Database;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
QSqlDatabase ISDatabase::GetDB(const QString &ConnectionName)
{
    if (QSqlDatabase::contains(ConnectionName))
    {
        return QSqlDatabase::database(ConnectionName);
    }
    return QSqlDatabase();
}
//-----------------------------------------------------------------------------
ISConnectOptionDB ISDatabase::GetOption(const QString &ConnectionName)
{
    ISConnectOptionDB ConnectOption;
    if (ConnectOptions.count(ConnectionName))
    {
        ConnectOption.Host = ConnectOptions[ConnectionName].Host;
        ConnectOption.Port = ConnectOptions[ConnectionName].Port;
        ConnectOption.Name = ConnectOptions[ConnectionName].Name;
        ConnectOption.Login = ConnectOptions[ConnectionName].Login;
        ConnectOption.Password = ConnectOptions[ConnectionName].Password;
    }
    return ConnectOption;
}
//-----------------------------------------------------------------------------
bool ISDatabase::CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist)
{
    ISQuery qSelectDatabase(GetDB(ConnectionName), QS_DATABASE);
    qSelectDatabase.SetShowLongQuery(false);
    qSelectDatabase.BindValue(":DatabaseName", Database);
    bool Result = qSelectDatabase.ExecuteFirst();
    if (Result)
    {
        Exist = qSelectDatabase.ReadColumn("count").toBool();
    }
    return Result;
}
//-----------------------------------------------------------------------------
QVariant ISDatabase::GetValue(const QString &TableName, const QString &FieldName, int ObjectID) const
{
    PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableName);
    PMetaField *MetaField = MetaTable->GetField(FieldName);
    QString SqlText = "SELECT " + MetaTable->Alias + '_' + MetaField->Name.toLower() + " FROM " + MetaTable->Name.toLower() + " WHERE " + MetaTable->Alias + "_id = :ObjectID";
    ISQuery qSelect(SqlText);
    qSelect.BindValue(":ObjectID", ObjectID);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn(MetaTable->Alias + '_' + MetaField->Name.toLower());
    }
    return QVariant();
}
//-----------------------------------------------------------------------------
bool ISDatabase::Connect(const QString &ConnectionName, const QString &Host, int Port, const QString &Database, const QString &Login, const QString &Password)
{
    //Если объект БД с таким именем соединения уже существует - то забираем
    //его из внутренней памяти QSqlDatabase, иначе - добавляем
    QSqlDatabase SqlDatabase = QSqlDatabase::contains(ConnectionName) ?
                QSqlDatabase::database(ConnectionName) :
                QSqlDatabase::addDatabase(SQL_DRIVER_QPSQL, ConnectionName);
    bool Result = SqlDatabase.isValid();
    if (!Result) //Ошибка при добавлении нового подключения в память
    {
        ErrorString = SqlDatabase.lastError().databaseText().simplified();
        return Result;
    }

    //Заполняем параметры подключения
    SqlDatabase.setHostName(Host);
    SqlDatabase.setPort(Port);
    SqlDatabase.setDatabaseName(Database);
    SqlDatabase.setUserName(Login);
    SqlDatabase.setPassword(Password);

    Result = SqlDatabase.open();
    if (!Result) //Ошибка при подключении
    {
        ErrorString = SqlDatabase.lastError().databaseText().simplified();
        return Result;
    }

    ConnectOptions.emplace(ConnectionName, ISConnectOptionDB{ Host, Port, Database, Login, Password });

    //Изменяем имя приложения для коннекта
    QSqlError SqlError = SqlDatabase.exec("SET application_name = '" + ISDefines::Core::APPLICATION_NAME + "'").lastError();
    if (SqlError.type() != QSqlError::NoError) //Не удалось изменить имя приложенения
    {
        ErrorString = "Not change application_name: " + SqlError.databaseText();
        return Result;
    }

    return Result;
}
//-----------------------------------------------------------------------------
void ISDatabase::Disconnect(const QString &ConnectionName)
{
    bool Contains = true; //Флаг удаления экземпляра БД из памяти
    {
        Contains = QSqlDatabase::contains(ConnectionName);
        if (Contains) //Если БД существует - проводим процедуру отключения от неё
        {
            QSqlDatabase SqlDatabase = QSqlDatabase::database(ConnectionName);
            if (SqlDatabase.isOpen())
            {
                SqlDatabase.close();
                SqlDatabase.setHostName(QString());
                SqlDatabase.setPort(0);
                SqlDatabase.setDatabaseName(QString());
                SqlDatabase.setUserName(QString());
                SqlDatabase.setPassword(QString());
            }
        }
    }

    if (Contains)
    {
        QSqlDatabase::removeDatabase(ConnectionName);
        ConnectOptions.erase(ConnectionName);
    }
}
//-----------------------------------------------------------------------------
void ISDatabase::DisconnectAll()
{
    for (const QString &ConnectionName : QSqlDatabase::connectionNames())
    {
        if (GetDB(ConnectionName).isOpen())
        {
            Disconnect(ConnectionName);
        }
    }
}
//-----------------------------------------------------------------------------
