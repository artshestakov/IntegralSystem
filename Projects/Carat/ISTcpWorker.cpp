#include "ISTcpWorker.h"
#include "ISAlgorithm.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISQueryText.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISTrace.h"
#include "ISTcpQueue.h"
//-----------------------------------------------------------------------------
static QString QS_AUTH = PREPARE_QUERY("SELECT usrs_id, usrs_issystem, usrs_isdeleted, usrs_group, usrs_fio, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, usgp_fullaccess "
									   "FROM _users "
									   "LEFT JOIN _usergroup ON usgp_id = usrs_group "
									   "WHERE usrs_login = :Login");
//-----------------------------------------------------------------------------
static QString QS_SETTINGS_DATABASE = PREPARE_QUERY("SELECT sgdb_settingname, sgdb_useraccessdatabase, sgdb_numbersimbolsaftercomma, sgdb_storagefilemaxsize "
													"FROM _settingsdatabase "
													"WHERE NOT sgdb_isdeleted "
													"AND sgdb_active");
//-----------------------------------------------------------------------------
static QString QS_SYSTEM_SUBSYSTEM = PREPARE_QUERY("SELECT "
												   "stms_issystem, stms_uid, stms_localname, stms_icon, stms_hint, "
												   "sbsm_uid, sbsm_localname, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
												   "FROM _subsystems "
												   "LEFT JOIN _systems ON stms_uid = sbsm_system "
												   "WHERE NOT sbsm_isdeleted "
												   "AND check_access_user_subsystem(:UserID, sbsm_uid) "
												   "ORDER BY stms_orderid, sbsm_orderid");
//-----------------------------------------------------------------------------
static QString QS_PRINTING = PREPARE_QUERY("SELECT rprt_uid, rprt_type, rprt_tablename, rprt_localname, rprt_filetemplate, "
										   "rprt_parent, rprt_replacevalue, rprt_sqlquery "
										   "FROM _report "
										   "WHERE NOT rprt_isdeleted "
										   "ORDER BY rprt_id");
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
										  "WHERE NOT sgts_isdeleted "
										  "AND sgts_creationuser = :UserID");
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE = PREPARE_QUERY("SELECT clsz_tablename, clsz_fieldname, clsz_size "
											  "FROM _columnsize "
											  "WHERE clsz_creationuser = :UserID");
//-----------------------------------------------------------------------------
static QString QS_USER_SETTINGS = PREPARE_QUERY("SELECT "
												"stgp_uid, stgp_name, stgp_localname, stgp_iconname, stgp_hint, "
												"stgs_uid, stgs_name, stgs_type, stgs_widgeteditname, stgs_localname, stgs_hint, stgs_defaultvalue, "
												"usst_value, "
												"(SELECT COUNT(*) FROM _usersettings WHERE usst_creationuser = :UserID AND usst_setting = stgs_id) "
												"FROM _settings "
												"LEFT JOIN _settingsgroup ON stgp_uid = stgs_group "
												"LEFT JOIN _usersettings ON usst_setting = stgs_id AND usst_creationuser = :UserID "
												"WHERE NOT stgs_isdeleted "
												"AND NOT stgp_isdeleted "
												"ORDER BY stgp_order, stgs_order");
//-----------------------------------------------------------------------------
static QString QI_USER_SETTING = PREPARE_QUERY("INSERT INTO _usersettings(usst_setting, usst_value) "
											   "VALUES((SELECT stgs_id FROM _settings WHERE stgs_uid = :SettingUID), :Value)");
//-----------------------------------------------------------------------------
static QString QS_PARAGRAPH = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname "
											"FROM _paragraphs "
											"WHERE NOT prhs_isdeleted "
											"ORDER BY prhs_orderid");
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
	DBConnectionName = QString::number((unsigned long)QThread::currentThreadId());

	//Пытаемся подключиться к БД
	IsStarted = ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword);
	if (!IsStarted)
	{
		ISLOGGER_E(__CLASS__, "Not connected to database: " + ISDatabase::Instance().GetErrorString());
	}

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
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsStopped = true;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Process()
{
	while (true)
	{
		ISSleep(1); //Засыпаем на одну милисекунду и даём поработать потоку

		//Проверяем, не остановлен ли воркер
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_stopped = IsStopped;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (is_stopped) //Если флаг остановки установлен - выходим из цикла
		{
			ISLOGGER_I(__CLASS__, QString("Stopping worker %1...").arg(DBConnectionName));
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
			if (!Result) //Запрос выполнен с ошибкой - устанавливаем текст ошибки в ответе и лог-сообщении
			{
				TcpAnswer->SetError(ErrorString);
				LogText.append("\nError string: " + ErrorString);
			}

			//Логируемся, добавляем ответ в очередь ответов и завершаем работу
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
			ErrorString = "field \"" + FieldName + "\" is empty";
		}
	}
	else
	{
		ErrorString = "field \"" + FieldName + "\" not exist";
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	//Проверяем, не авторизаван ли уже клиент. Если авторизован - выходим с ошибкой
	if (TcpMessage->TcpSocket->GetAuthorized())
	{
		ErrorString = LANG("Carat.Error.Query.AlreadyAuthorized");
		return false;
	}

	QVariant Login = CheckNullField("Login", TcpMessage->Parameters),
		Password = CheckNullField("Password", TcpMessage->Parameters);
	if (!Login.isValid() || !Password.isValid())
	{
		return false;
	}
	QString LoginString = Login.toString(),
		PasswordString = Password.toString();

	//Проверка пользователя
	ISQuery qSelectAuth(ISDatabase::Instance().GetDB(DBConnectionName), QS_AUTH);
	qSelectAuth.BindValue(":Login", LoginString);
	if (!qSelectAuth.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.SelectLogin").arg(qSelectAuth.GetErrorString());
		return false;
	}

	if (!qSelectAuth.ExecuteFirst())
	{
		ErrorString = LANG("Carat.Error.Query.NotFoundLogin").arg(LoginString);
		return false;
	}

	int UserID = qSelectAuth.ReadColumn("usrs_id").toInt();
	bool IsSystem = qSelectAuth.ReadColumn("usrs_issystem").toBool();
	bool IsDeleted = qSelectAuth.ReadColumn("usrs_isdeleted").toBool();
	QString UserFIO = qSelectAuth.ReadColumn("usrs_fio").toString();
	int GroupID = qSelectAuth.ReadColumn("usrs_group").toInt();
	bool GroupFullAccess = qSelectAuth.ReadColumn("usgp_fullaccess").toBool();

	//Если такой логин помечен на удаление
	if (IsDeleted)
	{
		ErrorString = LANG("Carat.Error.Query.LoginIsDeleted").arg(LoginString);
		return false;
	}

	//Если у пользователя нет права доступа
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.LoginNoAccess");
		return false;
	}

	//Проверка наличия привязки пользователя к группе
	if (!IsSystem) //Если пользователь не системный - проверяем привязку
	{
		if (GroupID == 0) //Привязка отсутствует
		{
			ErrorString = LANG("Carat.Error.Query.LoginLinkGroup");
			return false;
		}
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
				ErrorString = LANG("Carat.Error.Query.LoginLifetimeNotStarted").arg(DateStart.toString(FORMAT_DATE_V1));
				return false;
			}
			else if (CurrentDate > DateEnd)
			{
				ErrorString = LANG("Carat.Error.Query.LoginLifetimeEnded");
				return false;
			}
		}
	}

	//Проверяем подключение к БД. Если не удалось подключиться - ошибка
	QString TestConnectionName = ISSystem::GenerateUuid();
	if (!ISDatabase::Instance().Connect(TestConnectionName, DBHost, DBPort, DBName, LoginString, PasswordString))
	{
		ErrorString = LANG("Carat.Error.Query.DatabaseConnection").arg(ISDatabase::Instance().GetErrorString());
		return false;
	}

	//Подключились к БД - отключаемся от неё и устанавливаем флаги авторизации
	ISDatabase::Instance().Disconnect(TestConnectionName);
	TcpMessage->TcpSocket->SetAuthorized(true);
	TcpMessage->TcpSocket->SetUserID(UserID);

	//Отдаём информацию о пользователе и выходим из функции
	TcpAnswer->Parameters["UserID"] = UserID;
	TcpAnswer->Parameters["IsSystem"] = IsSystem;
	TcpAnswer->Parameters["FIO"] = UserFIO;
	TcpAnswer->Parameters["GroupID"] = GroupID;
	TcpAnswer->Parameters["GroupFullAccess"] = GroupFullAccess;
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
	QVariant Login = CheckNullField("Login", TcpMessage->Parameters);
	if (!Login.isValid())
	{
		return false;
	}
	QString LoginString = Login.toString();

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

	//Получаем системы и подсистемы
	QVariantMap SystemSubSystemMap;
	ISQuery qSelectSystems(ISDatabase::Instance().GetDB(DBConnectionName), QS_SYSTEM_SUBSYSTEM);
	qSelectSystems.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectSystems.Execute())
	{
		while (qSelectSystems.Next())
		{
			ISUuid SystemUID = qSelectSystems.ReadColumn("stms_uid");
			if (SystemSubSystemMap.contains(SystemUID)) //Если такая система уже была добавлена в Map - добавляем подсистему
			{
				QVariantMap SystemMap = SystemSubSystemMap[SystemUID].toMap();
				QVariantList SubSystemList = SystemMap["SubSystems"].toList();
				SubSystemList.append(QVariantMap
				{
					{ "UID", ISUuid(qSelectSystems.ReadColumn("sbsm_uid")) },
					{ "LocalName", qSelectSystems.ReadColumn("sbsm_localname") },
					{ "Icon", qSelectSystems.ReadColumn("sbsm_icon") },
					{ "ClassName", qSelectSystems.ReadColumn("sbsm_classname") },
					{ "TableName", qSelectSystems.ReadColumn("sbsm_tablename") },
					{ "Hint", qSelectSystems.ReadColumn("sbsm_hint") }
				});
				SystemMap["SubSystems"] = SubSystemList;
				SystemSubSystemMap[SystemUID] = SystemMap;
			}
			else //Такой систему в Map'е ещё нет - добавляем её
			{
				SystemSubSystemMap[SystemUID] = QVariantMap
				{
					{ "IsSystem", qSelectSystems.ReadColumn("stms_issystem") },
					{ "LocalName", qSelectSystems.ReadColumn("stms_localname") },
					{ "Icon", qSelectSystems.ReadColumn("stms_icon") },
					{ "Hint", qSelectSystems.ReadColumn("stms_hint") },
					{ "SubSystems", QVariantList() },
				};
			}
		}
		TcpAnswer->Parameters["SystemSubSystem"] = SystemSubSystemMap;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.MetaSystems").arg(qSelectSystems.GetErrorString());
		return false;
	}

	//Получаем печать
	QVariantMap PrintingMap;
	ISQuery qSelectPrinting(ISDatabase::Instance().GetDB(DBConnectionName), QS_PRINTING);
	if (qSelectPrinting.Execute())
	{
		while (qSelectPrinting.Next())
		{
			ISUuid UID = qSelectPrinting.ReadColumn("rprt_uid"),
				Parent = qSelectPrinting.ReadColumn("rprt_parent");
			if (Parent.isEmpty())
			{
				PrintingMap[UID] = QVariantMap
				{
					{ "Type", qSelectPrinting.ReadColumn("rprt_type") },
					{ "TableName", qSelectPrinting.ReadColumn("rprt_tablename") },
					{ "LocalName", qSelectPrinting.ReadColumn("rprt_localname") },
					{ "FileTemplate", qSelectPrinting.ReadColumn("rprt_filetemplate") },
					{ "Fields", QVariantMap() }
				};
			}
			else
			{
				QVariantMap ReportMap = PrintingMap[Parent].toMap();
				QVariantMap FieldsMap = ReportMap["Fields"].toMap();
				FieldsMap["ReplaceValue"] = qSelectPrinting.ReadColumn("rprt_replacevalue");
				FieldsMap["SqlQuery"] = qSelectPrinting.ReadColumn("rprt_sqlquery");
				ReportMap["Fields"] = FieldsMap;
				PrintingMap[Parent] = ReportMap;
			}
		}
		TcpAnswer->Parameters["Printing"] = PrintingMap;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Printing").arg(qSelectPrinting.GetErrorString());
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
			ObjectsID.remove(0, 1);
			ObjectsID.chop(1);
			FavoriteMap[TableName] = ObjectsID.split(SYMBOL_COMMA);
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
				{ "CreationDate", qSelectHistory.ReadColumn("htry_creationdate") },
				{ "TableName", qSelectHistory.ReadColumn("htry_tablename") },
				{ "ObjectID", qSelectHistory.ReadColumn("htry_objectid") }
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
				{ "TableName", qSelectSorting.ReadColumn("sgts_tablename") },
				{ "FieldName", qSelectSorting.ReadColumn("sgts_fieldname") },
				{ "Sorting", qSelectSorting.ReadColumn("sgts_sorting") }
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
				{ "TableName", qSelectColumnSize.ReadColumn("clsz_tablename") },
				{ "FieldName", qSelectColumnSize.ReadColumn("clsz_fieldname") },
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
	QVariantMap UserSettingsList;
	ISQuery qSelectUserSetting(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_SETTINGS);
	qSelectUserSetting.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectUserSetting.Execute())
	{
		while (qSelectUserSetting.Next())
		{
			ISUuid GroupUID = qSelectUserSetting.ReadColumn("stgp_uid"),
				SettingUID = qSelectUserSetting.ReadColumn("stgs_uid");
			if (!UserSettingsList.contains(GroupUID)) //Если такой группы нет - вставляем
			{
				UserSettingsList[GroupUID] = QVariantMap
				{
					{ "Name", qSelectUserSetting.ReadColumn("stgp_name") },
					{ "LocalName", qSelectUserSetting.ReadColumn("stgp_localname") },
					{ "IconName", qSelectUserSetting.ReadColumn("stgp_iconname") },
					{ "Hint", qSelectUserSetting.ReadColumn("stgp_hint") },
					{ "Settings", QVariantList() }
				};
			}
			QVariantMap GroupMap = UserSettingsList[GroupUID].toMap(); //Забираем группу по идентификатору
			QVariantList SettingsList = GroupMap["Settings"].toList(); //Забираем список настроек группы

			//Формируем объект с данными по настройке
			QVariantMap Setting =
			{
				{ "UID", SettingUID },
				{ "Name", qSelectUserSetting.ReadColumn("stgs_name") },
				{ "Type", qSelectUserSetting.ReadColumn("stgs_type") },
				{ "WidgetEditName", qSelectUserSetting.ReadColumn("stgs_widgeteditname") },
				{ "LocalName", qSelectUserSetting.ReadColumn("stgs_localname") },
				{ "Hint", qSelectUserSetting.ReadColumn("stgs_hint") },
				{ "DefaultValue", qSelectUserSetting.ReadColumn("stgs_defaultvalue") }
			};

			if (qSelectUserSetting.ReadColumn("count").toInt()) //Если такая настройка у пользователя уже есть - получаем её значение
			{
				Setting["Value"] = qSelectUserSetting.ReadColumn("usst_value");
			}
			else //Такой настройки у пользователя нет - добавляем
			{
				QVariant SettingDefaultValue = qSelectUserSetting.ReadColumn("stgs_defaultvalue");
				ISQuery qInsertSetting(ISDatabase::Instance().GetDB(DBConnectionName), QI_USER_SETTING);
				qInsertSetting.BindValue(":SettingUID", SettingUID);
				qInsertSetting.BindValue(":Value", SettingDefaultValue);
				if (!qInsertSetting.Execute())
				{
					ErrorString = "Error inserting new user setting: " + qInsertSetting.GetErrorString();
					return false;
				}
				Setting["Value"] = SettingDefaultValue;
			}

			SettingsList.append(Setting);
			GroupMap["Settings"] = SettingsList;
			UserSettingsList[GroupUID] = GroupMap;
		}
		TcpAnswer->Parameters["UserSettings"] = UserSettingsList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.UserSettings").arg(qSelectUserSetting.GetErrorString());
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
				{ "UID", ISUuid(qSelectParagraph.ReadColumn("prhs_uid")) },
				{ "Name", qSelectParagraph.ReadColumn("prhs_name") },
				{ "LocalName", qSelectParagraph.ReadColumn("prhs_localname") },
				{ "ToolTip", qSelectParagraph.ReadColumn("prhs_tooltip") },
				{ "Icon", qSelectParagraph.ReadColumn("prhs_icon") },
				{ "ClassName", qSelectParagraph.ReadColumn("prhs_classname") }
			});
		}
		TcpAnswer->Parameters["Paragraphs"] = ParagraphList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Paragraph").arg(qSelectParagraph.GetErrorString());
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
