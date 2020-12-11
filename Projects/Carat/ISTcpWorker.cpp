#include "ISTcpWorker.h"
#include "ISAlgorithm.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISTrace.h"
#include "ISTcpQueue.h"
#include "ISVersionInfo.h"
#include "ISConfig.h"
#include "ISTcpClients.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
static QString QS_USER_HASH = PREPARE_QUERY("SELECT usrs_hash, usrs_salt "
											"FROM _users");
//-----------------------------------------------------------------------------
static QString QS_USER_AUTH = PREPARE_QUERY("SELECT usrs_id, usrs_issystem, usrs_group, usrs_fio, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, usgp_fullaccess, "
											"(SELECT sgdb_useraccessdatabase FROM _settingsdatabase WHERE sgdb_active) "
											"FROM _users "
											"LEFT JOIN _usergroup ON usgp_id = usrs_group "
											"WHERE usrs_hash = :Hash");
//-----------------------------------------------------------------------------
static QString QI_PROTOCOL = PREPARE_QUERY("INSERT INTO _protocol(prtc_creationdate, prtc_creationuser, prtc_tablename, prtc_tablelocalname, prtc_type, prtc_objectid, prtc_information) "
										   "VALUES(:CreationDate, :CreationUser, :TableName, :TableLocalName, (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = :TypeUID), :ObjectID, :Information)");
//-----------------------------------------------------------------------------
static QString QS_SETTINGS_DATABASE = PREPARE_QUERY("SELECT sgdb_settingname, sgdb_useraccessdatabase, sgdb_numbersimbolsaftercomma, sgdb_storagefilemaxsize "
													"FROM _settingsdatabase "
													"WHERE sgdb_active");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_TABLE = PREPARE_QUERY("SELECT gatb_table, gatt_uid "
													 "FROM _groupaccesstable "
													 "LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_AccessType "
													 "WHERE gatb_group = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("SELECT gast_uid "
													   "FROM _groupaccessspecial "
													   "LEFT JOIN _groupaccessspecialtype ON gast_id = gasp_specialaccess "
													   "WHERE gasp_group = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_SYSTEM = PREPARE_QUERY("SELECT stms_issystem, stms_uid, stms_localname, stms_icon, stms_hint "
										 "FROM _systems "
										 "ORDER BY stms_orderid");
//-----------------------------------------------------------------------------
static QString QS_SUBSYSTEM = PREPARE_QUERY("SELECT sbsm_uid, sbsm_localname, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
											"FROM _subsystems "
											"WHERE sbsm_system = :SystemUID "
											"AND check_access_user_subsystem(:UserID, :UserIsSystem, sbsm_uid) "
											"ORDER BY sbsm_orderid");
//-----------------------------------------------------------------------------
static QString QS_REPORT = PREPARE_QUERY("SELECT rprt_uid, rprt_type, rprt_tablename, rprt_localname, rprt_filetemplate "
										 "FROM _report "
										 "WHERE rprt_parent IS NULL "
										 "ORDER BY rprt_id");
//-----------------------------------------------------------------------------
static QString QS_REPORT_FIELD = PREPARE_QUERY("SELECT rprt_replacevalue, rprt_sqlquery "
											   "FROM _report "
											   "WHERE rprt_parent = :ParentUID");
//-----------------------------------------------------------------------------
static QString QS_FAVORITE = PREPARE_QUERY("SELECT fvts_tablename, fvts_objectsid "
										   "FROM _favorites "
										   "WHERE fvts_creationuser = :UserID");
//-----------------------------------------------------------------------------
static QString QS_HISTORY = PREPARE_QUERY("SELECT htry_creationdate, htry_tablename, htry_objectid "
										  "FROM _history "
										  "WHERE htry_creationuser = :UserID "
										  "ORDER BY htry_id");
//-----------------------------------------------------------------------------
static QString QS_SORTING = PREPARE_QUERY("SELECT sgts_tablename, sgts_fieldname, sgts_sorting "
										  "FROM _sortingtables "
										  "WHERE sgts_creationuser = :UserID");
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE = PREPARE_QUERY("SELECT clsz_tablename, clsz_fieldname, clsz_size "
											  "FROM _columnsize "
											  "WHERE clsz_creationuser = :UserID");
//-----------------------------------------------------------------------------
static QString QS_SETTING_GROUP = PREPARE_QUERY("SELECT stgp_uid, stgp_name, stgp_localname, stgp_iconname, stgp_hint "
												"FROM _settingsgroup "
												"ORDER BY stgp_order");
//-----------------------------------------------------------------------------
static QString QS_SETTING = PREPARE_QUERY("SELECT stgs_uid, stgs_name, stgs_type, stgs_widgeteditname, stgs_localname, stgs_hint, stgs_defaultvalue, "
										  "usst_value, "
										  "(SELECT COUNT(*) FROM _usersettings WHERE usst_creationuser = :UserID AND usst_setting = stgs_id) "
										  "FROM _settings "
										  "LEFT JOIN _usersettings ON usst_setting = stgs_id AND usst_creationuser = :UserID "
										  "WHERE stgs_group = :GroupUID "
										  "ORDER BY stgs_order");
//-----------------------------------------------------------------------------
static QString QI_USER_SETTING = PREPARE_QUERY("INSERT INTO _usersettings(usst_setting, usst_value) "
											   "VALUES((SELECT stgs_id FROM _settings WHERE stgs_uid = :SettingUID), :Value)");
//-----------------------------------------------------------------------------
static QString QS_PARAGRAPH = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname "
											"FROM _paragraphs "
											"ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
static QString QU_USER_HASH = PREPARE_QUERY("UPDATE _users SET "
											"usrs_hash = :Hash, "
											"usrs_salt = :Salt "
											"WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_USER_HASH_CHECK = PREPARE_QUERY("SELECT usrs_hash = :HashOld AS current_hash_is_valid, usrs_hash = :Hash AS hash_old_and_new_is_equal "
												  "FROM _users "
												  "WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QU_USER_HASH_RESET = PREPARE_QUERY("UPDATE _users SET "
												  "usrs_hash = NULL, "
												  "usrs_salt = NULL "
												  "WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_ASTERISK_RECORD = PREPARE_QUERY("SELECT ascl_uniqueid "
												  "FROM _asteriskcalls "
												  "WHERE ascl_id = :RecordID");
//-----------------------------------------------------------------------------
static QString QI_USER_PASSWORD_CHANGE = PREPARE_QUERY("INSERT INTO _userpasswordchanged(upcg_user, upcg_type) "
													   "VALUES(:UserID, (SELECT upct_id FROM _userpasswordchangedtype WHERE upct_uid = :TypeUID))");
//-----------------------------------------------------------------------------
static QString QS_USER_PASSWORD_IS_NULL = PREPARE_QUERY("SELECT "
														"( "
														"SELECT (COUNT(*) > 0)::BOOLEAN is_exist "
														"FROM _users "
														"WHERE usrs_id = :UserID "
														"AND usrs_hash IS NOT NULL "
														"AND usrs_salt IS NOT NULL "
														") "
														"FROM _users "
														"WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_USER_IS_SYSTEM = PREPARE_QUERY("SELECT usrs_issystem "
												 "FROM _users "
												 "WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_CLIENT = PREPARE_QUERY("SELECT usrs_fio "
										 "FROM _users "
										 "WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QI_DISCUSSION = PREPARE_QUERY("INSERT INTO _discussion(dson_tablename, dson_objectid, dson_message) "
											 "VALUES(:TableName, :ObjectID, :Message) "
											 "RETURNING dson_id");
//-----------------------------------------------------------------------------
static QString QU_DISCUSSION = PREPARE_QUERY("UPDATE _discussion SET "
											 "dson_message = :Message "
											 "WHERE dson_id = :DiscussionID");
//-----------------------------------------------------------------------------
static QString QI_DISCUSSION_COPY = PREPARE_QUERY("INSERT INTO _discussion(dson_tablename, dson_objectid, dson_message) "
												  "SELECT dson_tablename, dson_objectid, dson_message "
												  "FROM _discussion "
												  "WHERE dson_id = :DiscussionID "
												  "RETURNING dson_id");
//-----------------------------------------------------------------------------
ISTcpWorker::ISTcpWorker(const QString &db_host, int db_port, const QString &db_name, const QString &db_user, const QString &db_password)
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	DBHost(db_host),
	DBPort(db_port),
	DBName(db_name),
	DBUser(db_user),
	DBPassword(db_password),
	IsStarted(false),
	IsRunning(false),
	CurrentMessage(nullptr),
	IsStopped(false)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpWorker::~ISTcpWorker()
{
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetStarted() const
{
	return IsStarted;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRunning()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool is_running = IsRunning;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return is_running;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::SetMessage(ISTcpMessage *TcpMessage)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	CurrentMessage = TcpMessage;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Run()
{
	//Поток, связанный с этим воркером будет удалятсья из памяти после завершения
	connect(thread(), &QThread::finished, thread(), &QThread::deleteLater);

	//Формируем имя подключения к БД
	DBConnectionName = QString::number(CURRENT_THREAD_ID());
	
	//Пытаемся подключиться к БД
	IsStarted = ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword);
	if (!IsStarted)
	{
		ISLOGGER_E(__CLASS__, "Not connected to database: " + ISDatabase::Instance().GetErrorString());
	}

	qProtocol = new ISQuery(ISDatabase::Instance().GetDB(DBConnectionName), QI_PROTOCOL);

	//Сигналим об успехе или ошибке
	emit IsStarted ? StartedDone() : StartedFailed();
	if (IsStarted)
	{
		ISLOGGER_I(__CLASS__, "Started");
		Process();
	}
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Stop()
{
	bool is_running = true;
	while (is_running)
	{
		ISSleep(1);
		CRITICAL_SECTION_LOCK(&CriticalSection);
		is_running = IsRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
	}
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsStopped = true;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Process()
{
	while (true)
	{
		ISSleep(1); //Засыпаем на милисекунду и даём поработать другим потокам

		//Проверяем, не остановлен ли воркер
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_stopped = IsStopped;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (is_stopped) //Если флаг остановки установлен - выходим из цикла
		{
			ISLOGGER_I(__CLASS__, QString("Stopping %1...").arg(DBConnectionName));
			delete qProtocol;
			ISDatabase::Instance().Disconnect(DBConnectionName);
			break;
		}

		//Получаем текущее сообщение
		CRITICAL_SECTION_LOCK(&CriticalSection);
		ISTcpMessage *tcp_message = CurrentMessage;
		if (tcp_message) //Если сообщение есть - устанавливаем флаг работы воркера
		{
			IsRunning = true;
			CurrentMessage = nullptr;
		}
		CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//Если появилось сообщение на обработку - выполняем его
		if (tcp_message)
		{
			bool Result = false;
			long long PerfomanceMsec = 0;
			ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpSocket); //Заранее формируем ответ

			if (tcp_message->IsValid()) //Если сообщение валидное - переходим к выполнению
			{
				//Если запрос не авторизационный и клиент ещё не авторизовался - ошибка
				if (tcp_message->Type != ISNamespace::AMT_Auth && !tcp_message->TcpSocket->GetAuthorized())
				{
					ErrorString = LANG("Carat.Error.NotAuthorized");
				}
				else //Клиент авторизовался - продолжаем
				{
					ISTimePoint TimePoint = ISAlgorithm::GetTick(); //Запоминаем текущее время
					switch (tcp_message->Type)
					{
					case ISNamespace::AMT_Unknown: break;
					case ISNamespace::AMT_Auth: Result = Auth(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_Sleep: Result = Sleep(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetMetaData: Result = GetMetaData(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetLastClient: Result = GetLastClient(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserPasswordExist: Result = UserPasswordExist(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserPasswordCreate: Result = UserPasswordCreate(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserPasswordEdit: Result = UserPasswordEdit(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserPasswordReset: Result = UserPasswordReset(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetRecordCall: Result = GetRecordCall(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetClients: Result = GetClients(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_RecordDelete: Result = RecordDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionAdd: Result = DiscussionAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionEdit: Result = DiscussionEdit(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionCopy: Result = DiscussionCopy(tcp_message, TcpAnswer); break;
					}
					PerfomanceMsec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
				}
			}
			else //Сообщение не валидное
			{
				ErrorString = tcp_message->GetErrorString();
			}

			//Формируем лог-сообщение
			QString LogText = QString("%1 message \"%2\". ID: %3 Size: %4 Chunk: %5 Parse msec: %6 MSec: %7").
				arg(Result ? "Done" : "Failed").
				arg(tcp_message->TypeName).
				arg(tcp_message->MessageID).
				arg(tcp_message->Size).
				arg(tcp_message->ChunkCount).
				arg(tcp_message->ParseMSec).
				arg(PerfomanceMsec);
			if (!Result) //Запрос выполнен с ошибкой - устанавливаем текст ошибки в ответе и лог-сообщении, а так же очищаем потенциально не пустые параметры ответа
			{
				TcpAnswer->SetError(ErrorString);
				LogText.append("\nError string: " + ErrorString);
				ErrorString.clear();
				TcpAnswer->Parameters.clear();
			}

			//Удаляем сообщение, логируемся, добавляем ответ в очередь ответов и завершаем работу
			delete tcp_message;
			Result ? ISLOGGER_I(__CLASS__, LogText) : ISLOGGER_E(__CLASS__, LogText);
			emit Answer(TcpAnswer);
			Finish();
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Finish()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
QVariant ISTcpWorker::CheckNullField(const QString &FieldName, const QVariantMap &VariantMap)
{
	if (VariantMap.contains(FieldName))
	{
		QVariant Value = VariantMap[FieldName];
		if (!Value.toString().isEmpty())
		{
			return Value;
		}
		else
		{
			ErrorString = LANG("Carat.Error.FieldIsEmpty").arg(FieldName);
		}
	}
	else
	{
		ErrorString = LANG("Carat.Error.FieldNotExist").arg(FieldName);
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Protocol(int UserID, const ISUuid &ActionTypeUID, const QVariant &TableName, const QVariant &TableLocalName, const QVariant &ObjectID, const QVariant &Information)
{
	qProtocol->BindValue(":CreationDate", QDateTime::currentDateTime());
	qProtocol->BindValue(":CreationUser", UserID);
	qProtocol->BindValue(":TableName", TableName);
	qProtocol->BindValue(":TypeUID", ActionTypeUID);
	qProtocol->BindValue(":ObjectID", ObjectID);
	qProtocol->BindValue(":TableLocalName", TableLocalName);
	qProtocol->BindValue(":Information", Information);
	if (!qProtocol->Execute()) //Не удалось добавить запись в протокол
	{
		ISLOGGER_E(__CLASS__, "Not insert protocol: " + qProtocol->GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTcpWorker::UserPasswordChangeEvent(const QVariant &UserID, const ISUuid &ChangeTypeUID)
{
	ISQuery qInsertPasswordChanged(ISDatabase::Instance().GetDB(DBConnectionName), QI_USER_PASSWORD_CHANGE);
	qInsertPasswordChanged.BindValue(":UserID", UserID);
	qInsertPasswordChanged.BindValue(":TypeUID", ChangeTypeUID);
	if (!qInsertPasswordChanged.Execute()) //Не удалось зафиксировать изменение пароля
	{
		ISLOGGER_E(__CLASS__, "Not fixed user password change: " + qInsertPasswordChanged.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordExist(const QVariant &UserID, bool &Exist)
{
	ISQuery qSelectHashIsNull(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_PASSWORD_IS_NULL);
	qSelectHashIsNull.BindValue(":UserID", UserID);
	if (!qSelectHashIsNull.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.CheckExistUserPassword").arg(qSelectHashIsNull.GetErrorString());
		return false;
	}

	if (!qSelectHashIsNull.First()) //Не удалось перейти на первую строку, т.к. пользователя с таким UserID не существует
	{
		ErrorString = LANG("Carat.Error.UserNotExist").arg(UserID.toInt());
		return false;
	}
	Exist = qSelectHashIsNull.ReadColumn("is_exist").toBool();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserIsSystem(const QVariant &UserID, bool &IsSystem)
{
	ISQuery qSelectIsSystem(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_IS_SYSTEM);
	qSelectIsSystem.BindValue(":UserID", UserID);
	if (!qSelectIsSystem.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.CheckUserIsSystem").arg(qSelectIsSystem.GetErrorString());
		return false;
	}

	if (!qSelectIsSystem.First())
	{
		ErrorString = LANG("Carat.Error.UserNotExist").arg(UserID.toInt());
		return false;
	}
	IsSystem = qSelectIsSystem.ReadColumn("usrs_issystem").toBool();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	//Проверяем, не авторизаван ли уже клиент. Если авторизован - выходим с ошибкой
	if (TcpMessage->TcpSocket->GetAuthorized())
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AlreadyAuthorized");
		return false;
	}

	//Если дата запрета меньше чем текущая - не даём зайти в программу
	if (ISVersionInfo::Instance().ConfigurationInfo.DateExpired.isValid() &&
		QDate::currentDate() > ISVersionInfo::Instance().ConfigurationInfo.DateExpired)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AuthDenied");
		return false;
	}

	QVariant Hash = CheckNullField("Hash", TcpMessage->Parameters);
	if (!Hash.isValid())
	{
		return false;
	}
	QString HashString = Hash.toString();

	//Проверяем размер хэша
	if ((size_t)HashString.size() != CARAT_HASH_SIZE)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.InvalidHashSize");
		return false;
	}

	//Проверяем валидность хэша
	for (const QChar &Char : HashString)
	{
		int ASCII = (int)Char.toLatin1(); //Преобразовываем текущий символ в ASCII-код
		if ((ASCII >= 48 && ASCII <= 57) || (ASCII >= 65 && ASCII <= 70)) //Если текущий символ входит в диапазон [0-9] или [a-z] - все окей
		{
			continue;
		}
		else //Иначе - хэш невалидный
		{
			ErrorString = LANG("Carat.Error.Query.Auth.InvalidHash");
			return false;
		}
	}

	{
		//Запрашиваем все хэши из БД
		ISQuery qSelectHash(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_HASH);
		if (!qSelectHash.Execute()) //Ошибка запроса
		{
			ErrorString = LANG("Carat.Error.Query.Auth.SelectHash").arg(qSelectHash.GetErrorString());
			return false;
		}

		//Если запрос ничего не вернул, значит в БД нет ни одного пользователя
		if (qSelectHash.GetCountResultRows() == 0)
		{
			ErrorString = LANG("Carat.Error.Query.Auth.NoUsers");
			return false;
		}

		//Ищем пользователя
		bool IsFound = false;
		while (qSelectHash.Next())
		{
			//Получаем хэш и соль
			QString CurrentHash = qSelectHash.ReadColumn("usrs_hash").toString(),
				CurrentSalt = qSelectHash.ReadColumn("usrs_salt").toString();

			//Солим присланный хэш текущей солью
			QString HashResult = ISAlgorithm::SaltPassword(HashString, CurrentSalt);
			IsFound = HashResult == CurrentHash;
			if (IsFound) //Нашли
			{
				Hash = HashResult;
				break;
			}
		}

		if (!IsFound) //Не нашли пользователя
		{
			ErrorString = LANG("Carat.Error.Query.Auth.InvalidLoginOrPassword");
			return false;
		}
	}

	//Проверка пользователя
	ISQuery qSelectAuth(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_AUTH);
	qSelectAuth.BindValue(":Hash", Hash);
	if (!qSelectAuth.ExecuteFirst()) //Запрос выполнен с ошибкой
	{
		ErrorString = LANG("Carat.Error.Query.Auth.SelectLogin").arg(qSelectAuth.GetErrorString());
		return false;
	}

	int UserID = qSelectAuth.ReadColumn("usrs_id").toInt();
	bool IsSystem = qSelectAuth.ReadColumn("usrs_issystem").toBool();
	QString UserFIO = qSelectAuth.ReadColumn("usrs_fio").toString();
	int GroupID = qSelectAuth.ReadColumn("usrs_group").toInt();
	bool GroupFullAccess = qSelectAuth.ReadColumn("usgp_fullaccess").toBool();

	//Доступ к БД запрещен
	if (!qSelectAuth.ReadColumn("sgdb_useraccessdatabase").toBool() && !IsSystem)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.ConnectionBan");
		return false;
	}

	//Если у пользователя нет права доступа
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.Auth.LoginNoAccess");
		return false;
	}

	//Проверка наличия привязки не системного пользователя к группе
	if (!IsSystem && GroupID == 0)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.LoginLinkGroup");
		return false;
	}

	//Если для пользователя настроено ограничение срока действия учётной записи
	if (qSelectAuth.ReadColumn("usrs_accountlifetime").toBool())
	{
		QDate DateStart = qSelectAuth.ReadColumn("usrs_accountlifetimestart").toDate();
		QDate DateEnd = qSelectAuth.ReadColumn("usrs_accountlifetimeend").toDate();
		if (DateStart.isValid() && DateEnd.isValid()) //Если дата начала и окончания срока действия установлена
		{
			QDate CurrentDate = QDate::currentDate();
			if (CurrentDate < DateStart)
			{
				ErrorString = LANG("Carat.Error.Query.Auth.LoginLifetimeNotStarted").arg(DateStart.toString(FORMAT_DATE_V1));
				return false;
			}
			else if (CurrentDate > DateEnd)
			{
				ErrorString = LANG("Carat.Error.Query.Auth.LoginLifetimeEnded");
				return false;
			}
		}
	}

	//Устанавливаем флаги сокету
	TcpMessage->TcpSocket->SetAuthorized(true);
	TcpMessage->TcpSocket->SetUserID(UserID);
	TcpMessage->TcpSocket->SetUserGroupID(GroupID);
	TcpMessage->TcpSocket->SetUserIsSystem(IsSystem);

	//Проверяем версию клиента
	bool IsNeedUpdate = false; //По умолчанию флаг обновления должен быть false (вдруг клиент отправил невалидную версию)
	QVariant Version = TcpMessage->SystemParameters["Version"];
	if (Version.isValid()) //Если версия указана
	{
		//Проверяем его версию на валидность
		QString VersionClientString = Version.toString();
		ISVersion VersionClient(VersionClientString);
		if (VersionClient.IsValid()) //Версия валидна
		{
			IsNeedUpdate = VersionClient < ISVersionInfo::Instance().Info.Version;
		}
		else //Версия невалидна
		{
			ISLOGGER_W(__CLASS__, "Client version invalid: " + VersionClientString);
		}
	}

	//Отдаём информацию о пользователе и конфигурации
	TcpAnswer->Parameters["UserID"] = UserID;
	TcpAnswer->Parameters["UserIsSystem"] = IsSystem;
	TcpAnswer->Parameters["UserFIO"] = UserFIO;
	TcpAnswer->Parameters["UserGroupID"] = GroupID;
	TcpAnswer->Parameters["UserGroupFullAccess"] = GroupFullAccess;
	TcpAnswer->Parameters["IsNeedUpdate"] = IsNeedUpdate;
	TcpAnswer->Parameters["Configuration"] = QVariantMap
	{
		{ "UID", ISVersionInfo::Instance().ConfigurationInfo.UID },
		{ "Name", ISVersionInfo::Instance().ConfigurationInfo.Name },
		{ "Local", ISVersionInfo::Instance().ConfigurationInfo.LocalName },
		{ "Desktop", ISVersionInfo::Instance().ConfigurationInfo.DesktopForm },
		{ "DateExpired", ISVersionInfo::Instance().ConfigurationInfo.DateExpired },
		{ "Logo", ISVersionInfo::Instance().ConfigurationInfo.LogoName }
	};

    //Регистрируем пользователя в "онлайн", протоколируем и выходим
	ISTcpClients::Instance().Add(
		TcpMessage->TcpSocket->socketDescriptor(),
		UserID,
		TcpMessage->TcpSocket->peerAddress().toString(),
        TcpMessage->TcpSocket->peerPort());
	Protocol(UserID, CONST_UID_PROTOCOL_ENTER_APPLICATION, QVariant(), QVariant(), QVariant(), QVariant());
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);
	QVariant Timeout = CheckNullField("Timeout", TcpMessage->Parameters);
	if (!Timeout.isValid())
	{
		return false;
	}
	
	bool Ok = true;
	int TimeoutInt = Timeout.toInt(&Ok);
	if (!Ok) //Не удалось привести значение к числу
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueInvalid");
		return false;
	}

	if (!TimeoutInt) //Если значение равно нулю - ошибка
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueIsNull");
		return false;
	}

	if (TimeoutInt < 0) //Если значение меньше нуля - ошибка
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueIsNegative");
		return false;
	}

	ISSleep(TimeoutInt);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//Получаем настройки БД
	QVariantMap SettingsDBMap;
	ISQuery qSelectSettingsDB(ISDatabase::Instance().GetDB(DBConnectionName), QS_SETTINGS_DATABASE);
	if (qSelectSettingsDB.Execute())
	{
		if (qSelectSettingsDB.First())
		{
			SettingsDBMap["Name"] = qSelectSettingsDB.ReadColumn("sgdb_settingname");
			SettingsDBMap["UserAccessDatabase"] = qSelectSettingsDB.ReadColumn("sgdb_useraccessdatabase");
			SettingsDBMap["NumberSymbolsAfterComma"] = qSelectSettingsDB.ReadColumn("sgdb_numbersimbolsaftercomma");
			SettingsDBMap["StirageFileMaxSize"] = qSelectSettingsDB.ReadColumn("sgdb_storagefilemaxsize");
			TcpAnswer->Parameters["SettingsDB"] = SettingsDBMap;
		}
		else
		{
			ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsDBNotActive");
			return false;
		}
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsDB").arg(qSelectSettingsDB.GetErrorString());
		return false;
	}

	//Получаем права на таблицы
	QVariantMap AccessTablesMap;
	ISQuery qSelectAccessTables(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_ACCESS_TABLE);
	qSelectAccessTables.BindValue(":GroupID", TcpMessage->TcpSocket->GetUserGroupID());
	if (qSelectAccessTables.Execute())
	{
		while (qSelectAccessTables.Next())
		{
			QString TableName = qSelectAccessTables.ReadColumn("gatb_table").toString();
			ISUuid AccessUID = qSelectAccessTables.ReadColumn("gatt_uid");
			if (AccessTablesMap.contains(TableName))
			{
				QVariantList AccessList = AccessTablesMap[TableName].toList();
				AccessList.append(AccessUID);
				AccessTablesMap[TableName] = AccessList;
			}
			else
			{
				AccessTablesMap[TableName] = QVariantList{ AccessUID };
			}
		}
		TcpAnswer->Parameters["AccessTables"] = AccessTablesMap;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.UserGroupAccessTable").arg(qSelectAccessTables.GetErrorString());
		return false;
	}

	//Получаем специальные права
	QVariantList AccessSpecialList;
	ISQuery qSelectAccessSpecial(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_ACCESS_SPECIAL);
	qSelectAccessSpecial.BindValue(":GroupID", TcpMessage->TcpSocket->GetUserGroupID());
	if (qSelectAccessSpecial.Execute())
	{
		while (qSelectAccessSpecial.Next())
		{
			AccessSpecialList.append(qSelectAccessSpecial.ReadColumn("gast_uid"));
		}
		TcpAnswer->Parameters["AccessSpecial"] = AccessSpecialList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial").arg(qSelectAccessSpecial.GetErrorString());
		return false;
	}

	//Получаем системы и подсистемы
	QVariantList SystemSubSystemList;
	ISQuery qSelectSystem(ISDatabase::Instance().GetDB(DBConnectionName), QS_SYSTEM),
		qSelectSubSystem(ISDatabase::Instance().GetDB(DBConnectionName), QS_SUBSYSTEM);
	if (qSelectSystem.Execute()) //Запрашиваем системы
	{
		while (qSelectSystem.Next())
		{
			QVariantList SubSystemsList;
			qSelectSubSystem.BindValue(":SystemUID", qSelectSystem.ReadColumn("stms_uid"));
			qSelectSubSystem.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qSelectSubSystem.BindValue(":UserIsSystem", TcpMessage->TcpSocket->GetUserIsSystem());
			if (qSelectSubSystem.Execute()) //Запрашиваем подсистемы текущей системы
			{
				while (qSelectSubSystem.Next())
				{
					SubSystemsList.append(QVariantMap
					{
						{ "UID", qSelectSubSystem.ReadColumn("sbsm_uid") },
						{ "Local", qSelectSubSystem.ReadColumn("sbsm_localname") },
						{ "Icon", qSelectSubSystem.ReadColumn("sbsm_icon") },
						{ "Class", qSelectSubSystem.ReadColumn("sbsm_classname") },
						{ "Table", qSelectSubSystem.ReadColumn("sbsm_tablename") },
						{ "Hint", qSelectSubSystem.ReadColumn("sbsm_hint") }
					});
				}
			}
			else
			{
				ErrorString = LANG("Carat.Error.Query.GetMetaData.SubSystems").arg(qSelectSubSystem.GetErrorString());
				return false;
			}
			
			//Добавляем систему только если по ней разрешены какие-нибудь подсистемы
			if (qSelectSubSystem.GetCountResultRows() > 0)
			{
				SystemSubSystemList.append(QVariantMap
				{
					{ "UID", qSelectSystem.ReadColumn("stms_uid") },
					{ "IsSystem", qSelectSystem.ReadColumn("stms_issystem") },
					{ "Local", qSelectSystem.ReadColumn("stms_localname") },
					{ "Icon", qSelectSystem.ReadColumn("stms_icon") },
					{ "Hint", qSelectSystem.ReadColumn("stms_hint") },
					{ "SubSystems", SubSystemsList }
				});
			}
		}
		TcpAnswer->Parameters["SystemSubSystem"] = SystemSubSystemList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Systems").arg(qSelectSystem.GetErrorString());
		return false;
	}

	//Получаем печать
	QVariantList PrintingList;
	ISQuery qSelectReport(ISDatabase::Instance().GetDB(DBConnectionName), QS_REPORT),
		qSelectReportField(ISDatabase::Instance().GetDB(DBConnectionName), QS_REPORT_FIELD);
	if (qSelectReport.Execute())
	{
		while (qSelectReport.Next())
		{
			QVariantMap ReportFieldMap;
			qSelectReportField.BindValue(":ParentUID", qSelectReport.ReadColumn("rprt_uid"));
			if (qSelectReportField.Execute())
			{
				while (qSelectReportField.Next())
				{
					ReportFieldMap.insert(qSelectReportField.ReadColumn("rprt_replacevalue").toString(), qSelectReportField.ReadColumn("rprt_sqlquery"));
				}
			}
			else
			{
				ErrorString = LANG("Carat.Error.Query.GetMetaData.ReportField").arg(qSelectReportField.GetErrorString());
				return false;
			}
			PrintingList.append(QVariantMap
			{
				{ "Type", qSelectReport.ReadColumn("rprt_type") },
				{ "Table", qSelectReport.ReadColumn("rprt_tablename") },
				{ "Local", qSelectReport.ReadColumn("rprt_localname") },
				{ "File", qSelectReport.ReadColumn("rprt_filetemplate") },
				{ "Fields", ReportFieldMap }
			});
		}
		TcpAnswer->Parameters["Printing"] = PrintingList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Report").arg(qSelectReport.GetErrorString());
		return false;
	}

	//Получаем избранные объекты
	QVariantMap FavoriteMap;
	ISQuery qSelectFavorite(ISDatabase::Instance().GetDB(DBConnectionName), QS_FAVORITE);
	qSelectFavorite.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectFavorite.Execute())
	{
		while (qSelectFavorite.Next())
		{
			QString TableName = qSelectFavorite.ReadColumn("fvts_tablename").toString();
			QString ObjectsID = qSelectFavorite.ReadColumn("fvts_objectsid").toString();
			FavoriteMap[TableName] = ObjectsID.mid(1, ObjectsID.size() - 2).split(SYMBOL_COMMA);
		}
		TcpAnswer->Parameters["Favorite"] = FavoriteMap;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Favorite").arg(qSelectFavorite.GetErrorString());
		return false;
	}

	//Получаем историю
	QVariantList HistoryList;
	ISQuery qSelectHistory(ISDatabase::Instance().GetDB(DBConnectionName), QS_HISTORY);
	qSelectHistory.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectHistory.Execute())
	{
		while (qSelectHistory.Next())
		{
			HistoryList.append(QVariantMap
			{
				{ "Date", qSelectHistory.ReadColumn("htry_creationdate") },
				{ "Table", qSelectHistory.ReadColumn("htry_tablename") },
				{ "ID", qSelectHistory.ReadColumn("htry_objectid") }
			});
		}
		TcpAnswer->Parameters["History"] = HistoryList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.History").arg(qSelectHistory.GetErrorString());
		return false;
	}

	//Получаем сортировки
	QVariantList SortingList;
	ISQuery qSelectSorting(ISDatabase::Instance().GetDB(DBConnectionName), QS_SORTING);
	qSelectSorting.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectSorting.Execute())
	{
		while (qSelectSorting.Next())
		{
			SortingList.append(QVariantMap
			{
				{ "Table", qSelectSorting.ReadColumn("sgts_tablename") },
				{ "Field", qSelectSorting.ReadColumn("sgts_fieldname") },
				{ "Sort", qSelectSorting.ReadColumn("sgts_sorting") }
			});
		}
		TcpAnswer->Parameters["Sorting"] = SortingList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Sorting").arg(qSelectSorting.GetErrorString());
		return false;
	}

	//Получаем размеры колонок
	QVariantList ColumnSizeList;
	ISQuery qSelectColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QS_COLUMN_SIZE);
	qSelectColumnSize.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectColumnSize.Execute())
	{
		while (qSelectColumnSize.Next())
		{
			ColumnSizeList.append(QVariantMap
			{
				{ "Table", qSelectColumnSize.ReadColumn("clsz_tablename") },
				{ "Field", qSelectColumnSize.ReadColumn("clsz_fieldname") },
				{ "Size", qSelectColumnSize.ReadColumn("clsz_size") }
			});
		}
		TcpAnswer->Parameters["ColumnSize"] = ColumnSizeList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.ColumnSize").arg(qSelectColumnSize.GetErrorString());
		return false;
	}

	//Получаем пользовательские настройки
	QVariantList Settings;
	ISQuery qSelectSettingGroup(ISDatabase::Instance().GetDB(DBConnectionName), QS_SETTING_GROUP),
		qSelectSettingUser(ISDatabase::Instance().GetDB(DBConnectionName), QS_SETTING);
	if (qSelectSettingGroup.Execute())
	{
		while (qSelectSettingGroup.Next())
		{
			ISUuid GroupUID = qSelectSettingGroup.ReadColumn("stgp_uid");
			QVariantList SettingsList;
			qSelectSettingUser.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qSelectSettingUser.BindValue(":GroupUID", GroupUID);
			if (qSelectSettingUser.Execute())
			{
				while (qSelectSettingUser.Next())
				{
					ISUuid SettingUID = qSelectSettingUser.ReadColumn("stgs_uid");
					QVariant SettingDefault = qSelectSettingUser.ReadColumn("stgs_defaultvalue");
					QVariantMap SettingMap =
					{
						{ "UID", SettingUID },
						{ "Name", qSelectSettingUser.ReadColumn("stgs_name") },
						{ "Type", qSelectSettingUser.ReadColumn("stgs_type") },
						{ "WidgetEdit", qSelectSettingUser.ReadColumn("stgs_widgeteditname") },
						{ "Local", qSelectSettingUser.ReadColumn("stgs_localname") },
						{ "Hint", qSelectSettingUser.ReadColumn("stgs_hint") },
						{ "Default", SettingDefault }
					};

					if (qSelectSettingUser.ReadColumn("count").toInt())  //Если такая настройка у пользователя уже есть - получаем её значение
					{
						SettingMap["Value"] = qSelectSettingUser.ReadColumn("usst_value");
					}
					else //Такой настройки у пользователя нет - добавляем со значением по умолчанию
					{
						ISQuery qInsertSetting(ISDatabase::Instance().GetDB(DBConnectionName), QI_USER_SETTING);
						qInsertSetting.BindValue(":SettingUID", SettingUID);
						qInsertSetting.BindValue(":Value", SettingDefault);
						if (!qInsertSetting.Execute())
						{
							ErrorString = "Error inserting new user setting: " + qInsertSetting.GetErrorString();
							return false;
						}
						SettingMap["Value"] = SettingDefault;
					}
					SettingsList.append(SettingMap);
				}
			}
			else
			{
				ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsUser").arg(qSelectSettingUser.GetErrorString());
				return false;
			}
			Settings.append(QVariantMap
			{
				{ "UID", GroupUID },
				{ "Name", qSelectSettingGroup.ReadColumn("stgp_name").toString() },
				{ "Local", qSelectSettingGroup.ReadColumn("stgp_localname").toString() },
				{ "Icon", qSelectSettingGroup.ReadColumn("stgp_iconname").toString() },
				{ "Hint", qSelectSettingGroup.ReadColumn("stgp_hint").toString() },
				{ "Settings", SettingsList }
			});
		}
		TcpAnswer->Parameters["Settings"] = Settings;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsGroup").arg(qSelectSettingGroup.GetErrorString());
		return false;
	}

	//Получаем параграфы
	QVariantList ParagraphList;
	ISQuery qSelectParagraph(ISDatabase::Instance().GetDB(DBConnectionName), QS_PARAGRAPH);
	if (qSelectParagraph.Execute())
	{
		while (qSelectParagraph.Next())
		{
			ParagraphList.append(QVariantMap
			{
				{ "UID", qSelectParagraph.ReadColumn("prhs_uid") },
				{ "Name", qSelectParagraph.ReadColumn("prhs_name") },
				{ "Local", qSelectParagraph.ReadColumn("prhs_localname") },
				{ "ToolTip", qSelectParagraph.ReadColumn("prhs_tooltip") },
				{ "Icon", qSelectParagraph.ReadColumn("prhs_icon") },
				{ "Class", qSelectParagraph.ReadColumn("prhs_classname") }
			});
		}
		TcpAnswer->Parameters["Paragraphs"] = ParagraphList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Paragraph").arg(qSelectParagraph.GetErrorString());
		return false;
	}

	//Читаем мета-данные
	QVariantList MetaDataList;
	QStringList Filter("*.xsn"); //Фильтр
	QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //Загрузка мета-данных движка
	FileInfoList.append(QDir(":_" + ISVersionInfo::Instance().ConfigurationInfo.Name).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка мета-данных конфигурации
	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех XSN файлов
	{
		QFile FileXSN(FileInfo.filePath());
		if (!FileXSN.open(QIODevice::ReadOnly)) //Не удалось открыть файл на чтение - выходим с ошибкой
		{
			ErrorString = LANG("Carat.Error.Query.GetMetaData.FileXSN").arg(FileInfo.fileName()).arg(FileXSN.errorString());
			return false;
		}
		MetaDataList.append(FileXSN.readAll());
		FileXSN.close();
	}
	TcpAnswer->Parameters["MetaData"] = MetaDataList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Получаем директорию с дистрибутивами
	QString UpdateClientDir = CONFIG_STRING(CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR);
	if (UpdateClientDir.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.GetLastClient.PathIsEmpty");
		return false;
	}

	QDir Dir(UpdateClientDir);
	if (!Dir.exists())
	{
		ISLOGGER_W(__CLASS__, "Folder \"" + UpdateClientDir + "\" not exist. Check config parameter \"" + CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR + "\"");
		ErrorString = LANG("Carat.Error.Query.GetLastClient.Other");
		return false;
	}

	//Получаем отсортированный (по дате) список файлов и проверяем его на пустоту
	QStringList StringList = Dir.entryList(QDir::Files, QDir::Time);
	if (StringList.isEmpty()) //Если обновлений нет - выходим
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//Обходим список файлов и ищем самый последний
	QString FilePath;
	for (const QString &FileName : StringList)
	{
		QStringList FileNameList = FileName.split(SYMBOL_POINT);
		if (FileNameList.size() != 4) //Если имя файла невалидное - пропускаем его
		{
			continue;
		}
		FilePath = Dir.absolutePath() + '/' + FileName;
		break;
	}

	//Файл не найден - выходим
	if (FilePath.isEmpty())
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//Открываем файл
	QFile File(FilePath);
	if (!File.open(QIODevice::ReadOnly)) //Не удалось открыть файл
	{
		ISLOGGER_E(__CLASS__, "Not read file \"" + FilePath + "\": " + File.errorString());
		ErrorString = LANG("Carat.Error.Query.GetLastClient.Other");
		return false;
	}

	//Читаем и сразу закрываем файл
	QByteArray ByteArray = File.readAll();
	File.close();

	//Конвертируем в base64 и отдаём
	TcpAnswer->Parameters["FileName"] = QFileInfo(FilePath).fileName();
	TcpAnswer->Parameters["Data"] = ByteArray.toBase64();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant UserID = CheckNullField("UserID", TcpMessage->Parameters);
	if (!UserID.isValid())
	{
		return false;
	}

	//Проверяем наличие пароля
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist)) //Не удалось проверить наличие пароля
	{
		return false;
	}
	TcpAnswer->Parameters["IsExist"] = Exist;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordCreate(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage->Parameters),
		Hash = CheckNullField("Hash", TcpMessage->Parameters);
	if (!Hash.isValid() || !UserID.isValid())
	{
		return false;
	}

	//Проверяем пользователя на системность
	bool IsSystem = true;
	if (!UserIsSystem(UserID, IsSystem))
	{
		return false;
	}

	if (IsSystem) //Пользователь системный - отказываем в сбросе пароля
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.UserIsSystem");
		return false;
	}

	//Проверяем наличие пароля
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist)) //Не удалось проверить наличие пароля
	{
		return false;
	}

	if (Exist) //Пароль уже существует - считаем за ошибку
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.AlreadyExist");
		return false;
	}

	//Генерируем соль и солим пароль
	QString Salt;
	if (!ISAlgorithm::GenerateSalt(Salt, ErrorString))
	{
		return false;
	}
	QString HashResult = ISAlgorithm::SaltPassword(Hash.toString(), Salt);

	//Устанавливаем пароль
	ISQuery qUpdateHash(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH);
	qUpdateHash.BindValue(":Hash", HashResult);
	qUpdateHash.BindValue(":Salt", Salt);
	qUpdateHash.BindValue(":UserID", UserID);
	if (!qUpdateHash.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.UpdateHash").arg(qUpdateHash.GetErrorString());
		return false;
	}

	//Фиксируем изменение пароля в истории
	UserPasswordChangeEvent(UserID, CONST_UID_USER_PASSWORD_CREATE);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage->Parameters),
		HashOld = CheckNullField("HashOld", TcpMessage->Parameters),
		HashNew = CheckNullField("HashNew", TcpMessage->Parameters);
	if (!UserID.isValid() || !HashOld.isValid() || !HashNew.isValid())
	{
		return false;
	}

	//Проверяем правильность старого хэша и не равен ли новый хэш старому
	ISQuery qSelectHashCheck(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_HASH_CHECK);
	qSelectHashCheck.BindValue(":HashOld", HashOld);
	qSelectHashCheck.BindValue(":Hash", HashNew);
	qSelectHashCheck.BindValue(":UserID", UserID);
	if (!qSelectHashCheck.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.Equal").arg(qSelectHashCheck.GetErrorString());
		return false;
	}

	if (!qSelectHashCheck.First()) //Пользователя с таким UserID нет в БД
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.UserNotExist").arg(UserID.toInt());
		return false;
	}

	bool CurrentHashIsValid = qSelectHashCheck.ReadColumn("current_hash_is_valid").toBool(),
		HashOldAndNewIsEqual = qSelectHashCheck.ReadColumn("hash_old_and_new_is_equal").toBool();
	if (!CurrentHashIsValid) //Текущий пароль неправильный
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.InvalidCurrentPassword");
		return false;
	}

	if (HashOldAndNewIsEqual) //Если хэши равны - считаем это ошибкой
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.OldAndNewPasswordsEqual");
		return false;
	}

	//Обновляем пароль
	ISQuery qUpdateHash(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH);
	qUpdateHash.BindValue(":Hash", HashNew);
	qUpdateHash.BindValue(":UserID", UserID);
	if (!qUpdateHash.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginCreate.UpdateHash").arg(qUpdateHash.GetErrorString());
		return false;
	}

	//Фиксируем изменение пароля в истории
	UserPasswordChangeEvent(UserID, CONST_UID_USER_PASSWORD_UPDATE);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage->Parameters);
	if (!UserID.isValid())
	{
		return false;
	}

	//Проверяем пользователя на системность
	bool IsSystem = true;
	if (!UserIsSystem(UserID, IsSystem))
	{
		return false;
	}

	if (IsSystem) //Пользователь системный - отказываем в сбросе пароля
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.UserIsSystem");
		return false;
	}

	//Проверяем наличие пароля
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist))
	{
		return false;
	}
	
	if (!Exist) //Если пароля нет - считаем ошибкой
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.PasswordIsNull");
		return false;
	}

	//Сбрасываем пароль
	ISQuery qUpdateHashReset(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH_RESET);
	qUpdateHashReset.BindValue(":UserID", UserID);
	if (!qUpdateHashReset.Execute()) //Не удалось сбросить пароль
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.Reset");
		return false;
	}

	//Фиксируем изменение пароля в истории
	UserPasswordChangeEvent(UserID, CONST_UID_USER_PASSWORD_RESET);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//Получаем директорию с записями разговоров
	QString RecordDir = CONFIG_STRING(CONST_CONFIG_AMI_RECORD_DIR);
	if (RecordDir.isEmpty()) //Директория не настроена - ошибка
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.RecordDirIsEmpty");
		return false;
	}

	QVariant RecordID = CheckNullField("RecordID", TcpMessage->Parameters);
	if (!RecordID.isValid())
	{
		return false;
	}

	//Получаем идентификатор записи по её коду
	ISQuery qSelectRecord(ISDatabase::Instance().GetDB(DBConnectionName), QS_ASTERISK_RECORD);
	qSelectRecord.BindValue(":RecordID", RecordID);
	if (!qSelectRecord.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.SelectUniqueID").arg(qSelectRecord.GetErrorString());
		return false;
	}
	if (!qSelectRecord.First())
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.NotFoundUniqueID").arg(RecordID.toInt());
		return false;
	}
	QString UniqueID = qSelectRecord.ReadColumn("ascl_uniqueid").toString();
	
	QFile FileRecord(RecordDir + UniqueID + SYMBOL_POINT + EXTENSION_WAV);
	if (!FileRecord.exists()) //Файл не существует
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.NotFoundRecordCall");
		return false;
	}

	if (!FileRecord.open(QIODevice::ReadOnly)) //Не удалось открыть файл
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.OpenRecordCall").arg(FileRecord.errorString());
		return false;
	}

	//Читаем файл, закрываем и выходим
	TcpAnswer->Parameters["Data"] = FileRecord.readAll().toBase64();
	FileRecord.close();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetClients(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Получаем список подключенных клиентов, объявляем контейнер для них и запрос
	std::vector<ISClientInfo> VectorClients = ISTcpClients::Instance().GetClients();
	QVariantList VariantList;
	ISQuery qSelectClient(ISDatabase::Instance().GetDB(DBConnectionName), QS_CLIENT);

	//Обходим всех клиентов и получаем по каждому информацию
	for (const ISClientInfo &ClientInfo : VectorClients)
	{
		qSelectClient.BindValue(":UserID", ClientInfo.ID);
		if (!qSelectClient.Execute()) //Ошибка в запросе
		{
			ErrorString = LANG("Carat.Error.Query.GetClients.Select").arg(ClientInfo.ID);
			break;
		}

		if (!qSelectClient.First()) //Пользователь не найден
		{
			ErrorString = LANG("Carat.Error.Query.GetClients.UserNotExist").arg(ClientInfo.ID);
			break;
		}

		//Добавляем информацию о пользователе в список
		VariantList.push_back(QVariantMap
		{
			{ "Address", ClientInfo.Address },
			{ "Port", ClientInfo.Port },
			{ "FIO", qSelectClient.ReadColumn("usrs_fio") }
		});
	}
	
	//Если была ошибка - очищаем потенциально не пустой список и выходим
	if (qSelectClient.GetErrorType() != QSqlError::NoError)
	{
		VariantList.clear();
		return false;
	}
	TcpAnswer->Parameters["Clients"] = VariantList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant TableName = CheckNullField("TableName", TcpMessage->Parameters);
	if (!TableName.isValid())
	{
		return false;
	}
	QString TableNameString = TableName.toString();

	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableNameString);
	if (!MetaTable)
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.TableNotExist").arg(TableNameString);
		return false;
	}

	//Проверяем наличие идентификаторов
	QVariantList Objects = TcpMessage->Parameters["Objects"].toList();
	if (Objects.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.NotSpecifiedID");
		return false;
	}

	//Формируем конструкцию IN для условия WHERE
	bool Ok = true;
	QString SqlIN;
	for (const QVariant &ID : Objects)
	{
		SqlIN += QString::number(ID.toInt(&Ok)) + SYMBOL_COMMA;
		if (!Ok) //Не удалось привести к числу
		{
			ErrorString = LANG("Carat.Error.Query.RecordDelete.ToInt");
			return false;
		}
	}
	SqlIN.chop(1);

	//Проверяем, нет ли системных записей
	ISQuery qSqlQuery(ISDatabase::Instance().GetDB(DBConnectionName), "SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM " + TableNameString + " WHERE " + MetaTable->Alias + "_issystem AND " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	if (!qSqlQuery.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.Select").arg(qSqlQuery.GetErrorString());
		return false;
	}

	if (!qSqlQuery.First())
	{
		ErrorString = qSqlQuery.GetErrorString();
		return false;
	}

	if (qSqlQuery.ReadColumn("is_exist").toBool())
	{
		ErrorString = Objects.size() == 1 ?
			LANG("Carat.Error.Query.RecordDelete.RecordIsSystem") :
			LANG("Carat.Error.Query.RecordDelete.RecordsIsSystem");
		return false;
	}

	//Удаляем
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), "DELETE FROM " + TableNameString + " WHERE " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	if (!qDelete.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.Delete").arg(qDelete.GetErrorString());
		return false;
	}

	//Протоколируем
	int UseriD = TcpMessage->TcpSocket->GetUserID();
	for (const QVariant &ObjectID : Objects)
	{
		Protocol(UseriD, CONST_UID_PROTOCOL_DELETE_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID, QVariant());
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage->Parameters),
		ObjectID = CheckNullField("ObjectID", TcpMessage->Parameters),
		Message = CheckNullField("Message", TcpMessage->Parameters);
	if (!TableName.isValid() || !ObjectID.isValid() || !Message.isValid())
	{
		return false;
	}

	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_DISCUSSION);
	qInsert.BindValue(":TableName", TableName);
	qInsert.BindValue(":ObjectID", ObjectID);
	qInsert.BindValue(":Message", Message);
	if (!qInsert.Execute()) //Ошибка вставки
	{
		ErrorString = LANG("Carat.Error.Query.DiscussionAdd.Insert").arg(qInsert.GetErrorString());
		return false;
	}

	if (!qInsert.First()) //Ошибка перехода к возвращаемому значению
	{
		ErrorString = qInsert.GetErrorString();
		return false;
	}

	TcpAnswer->Parameters["ID"] = qInsert.ReadColumn("dson_id");
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant DiscussionID = CheckNullField("ID", TcpMessage->Parameters),
		Message = CheckNullField("Message", TcpMessage->Parameters);
	if (!DiscussionID.isValid() || !Message.isValid())
	{
		return false;
	}

	ISQuery qUpdate(ISDatabase::Instance().GetDB(DBConnectionName), QU_DISCUSSION);
	qUpdate.BindValue(":Message", Message);
	qUpdate.BindValue(":DiscussionID", DiscussionID);
	if (!qUpdate.Execute()) //Не удалось обновить запись
	{
		ErrorString = LANG("Carat.Error.Query.DiscussionEdit.Update").arg(qUpdate.GetErrorString());
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant DiscussionID = CheckNullField("ID", TcpMessage->Parameters);
	if (!DiscussionID.isValid())
	{
		return false;
	}

	ISQuery qCopy(ISDatabase::Instance().GetDB(DBConnectionName), QI_DISCUSSION_COPY);
	qCopy.BindValue(":DiscussionID", DiscussionID);
	if (!qCopy.Execute()) //Не удалось создать копию
	{
		ErrorString = LANG("Carat.Error.Query.DiscussionCopy.Insert").arg(qCopy.GetErrorString());
		return false;
	}

	if (!qCopy.First()) //Ошибка перехода к возвращаемому значению
	{
		ErrorString = qCopy.GetErrorString();
		return false;
	}
	TcpAnswer->Parameters["ID"] = qCopy.ReadColumn("dson_id");
	return true;
}
//-----------------------------------------------------------------------------
