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
#include "ISVersionInfo.h"
#include "ISConfig.h"
#include "ISQueryPool.h"
//-----------------------------------------------------------------------------
static QString QS_AUTH = PREPARE_QUERY("SELECT usrs_id, usrs_issystem, usrs_isdeleted, usrs_group, usrs_fio, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, usgp_fullaccess "
									   "FROM _users "
									   "LEFT JOIN _usergroup ON usgp_id = usrs_group "
									   "WHERE usrs_hash = :Hash");
//-----------------------------------------------------------------------------
static QString QI_PROTOCOL = PREPARE_QUERY("INSERT INTO _protocol(prtc_creationdate, prtc_creationuser, prtc_tablename, prtc_tablelocalname, prtc_type, prtc_objectid, prtc_information) "
										   "VALUES(:CreationDate, :CreationUser, :TableName, :TableLocalName, (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = :TypeUID), :ObjectID, :Information)");
//-----------------------------------------------------------------------------
static QString QS_SETTINGS_DATABASE = PREPARE_QUERY("SELECT sgdb_settingname, sgdb_useraccessdatabase, sgdb_numbersimbolsaftercomma, sgdb_storagefilemaxsize "
													"FROM _settingsdatabase "
													"WHERE NOT sgdb_isdeleted "
													"AND sgdb_active");
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
static QString QS_SYSTEM_SUBSYSTEM = PREPARE_QUERY("SELECT "
												   "stms_issystem, stms_uid, stms_localname, stms_icon, stms_hint, "
												   "sbsm_uid, sbsm_localname, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
												   "FROM _subsystems "
												   "LEFT JOIN _systems ON stms_uid = sbsm_system "
												   "WHERE NOT sbsm_isdeleted "
												   "AND check_access_user_subsystem(:UserID, :UserIsSystem, sbsm_uid) " //�������� ����������� ���� ���������� ������������
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
	//�����, ��������� � ���� �������� ����� ��������� �� ������ ����� ����������
	connect(thread(), &QThread::finished, thread(), &QThread::deleteLater);

	//��������� ��� ����������� � ��
	DBConnectionName = QString::number(CURRENT_THREAD_ID());
	
	//�������� ������������ � ��
	IsStarted = ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword);
	if (!IsStarted)
	{
		ISLOGGER_E(__CLASS__, "Not connected to database: " + ISDatabase::Instance().GetErrorString());
	}

	//�������� �� ������ ��� ������
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
		ISSleep(1); //�������� �� ����������� � ��� ���������� ������ �������

		//���������, �� ���������� �� ������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_stopped = IsStopped;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (is_stopped) //���� ���� ��������� ���������� - ������� �� �����
		{
			ISLOGGER_I(__CLASS__, QString("Stopping %1...").arg(DBConnectionName));
			ISDatabase::Instance().Disconnect(DBConnectionName);
			break;
		}

		//�������� ������� ���������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		ISTcpMessage *tcp_message = CurrentMessage;
		if (tcp_message) //���� ��������� ���� - ������������� ���� ������ �������
		{
			IsRunning = true;
			CurrentMessage = nullptr;
		}
		CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//���� ��������� ��������� �� ��������� - ��������� ���
		if (tcp_message)
		{
			bool Result = false;
			long long PerfomanceMsec = 0;
			ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpSocket); //������� ��������� �����

			if (tcp_message->IsValid()) //���� ��������� �������� - ��������� � ����������
			{
				//���� ������ �� ��������������� � ������ ��� �� ������������� - ������
				if (tcp_message->Type != ISNamespace::AMT_Auth && !tcp_message->TcpSocket->GetAuthorized())
				{
					ErrorString = LANG("Carat.Error.NotAuthorized");
				}
				else //������ ������������� - ����������
				{
					ISTimePoint TimePoint = ISAlgorithm::GetTick(); //���������� ������� �����
					switch (tcp_message->Type)
					{
					case ISNamespace::AMT_Unknown: break;
					case ISNamespace::AMT_Auth: Result = Auth(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_Sleep: Result = Sleep(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetMetaData: Result = GetMetaData(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_Test: Result = Test(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetLastClient: Result = GetLastClient(tcp_message, TcpAnswer); break;
					}
					PerfomanceMsec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
				}
			}
			else //��������� �� ��������
			{
				ErrorString = tcp_message->GetErrorString();
			}

			//��������� ���-���������
			QString LogText = QString("%1 message \"%2\". ID: %3 Size: %4 Chunk: %5 Parse msec: %6 MSec: %7").
				arg(Result ? "Done" : "Failed").
				arg(tcp_message->TypeName).
				arg(tcp_message->MessageID).
				arg(tcp_message->Size).
				arg(tcp_message->ChunkCount).
				arg(tcp_message->ParseMSec).
				arg(PerfomanceMsec);
			if (!Result) //������ �������� � ������� - ������������� ����� ������ � ������ � ���-���������
			{
				TcpAnswer->SetError(ErrorString);
				LogText.append("\nError string: " + ErrorString);
			}

			//������� ���������, ����������, ��������� ����� � ������� ������� � ��������� ������
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
void ISTcpWorker::Protocol(int UserID, const ISUuid &ActionTypeUID, const QString &TableName, const QString &TableLocalName, const QVariant &ObjectID, const QString &Information)
{
	ISQueryPool::Instance().AddQuery(QI_PROTOCOL, ISStringToVariantMap
	{
		{ ":CreationDate", QDateTime::currentDateTime() },
		{ ":CreationUser", UserID },
		{ ":TableName", TableName },
		{ ":TypeUID", ActionTypeUID },
		{ ":ObjectID", ObjectID },
		{ ":TableLocalName", TableLocalName },
		{ ":Information", Information }
	});
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	//���������, �� ����������� �� ��� ������. ���� ����������� - ������� � �������
	if (TcpMessage->TcpSocket->GetAuthorized())
	{
		ErrorString = LANG("Carat.Error.Query.AlreadyAuthorized");
		return false;
	}

	QVariant Hash = CheckNullField("Hash", TcpMessage->Parameters),
		Version = CheckNullField("Version", TcpMessage->Parameters);
	if (!Hash.isValid())
	{
		return false;
	}
	QString HashString = Hash.toString();

	//��������� ������ ����
	if ((size_t)HashString.size() != CARAT_HASH_SIZE)
	{
		ErrorString = LANG("Carat.Error.Query.InvalidHashSize");
		return false;
	}

	//��������� ���������� ����
	for (const QChar &Char : HashString)
	{
		int ASCII = (int)Char.toLatin1(); //��������������� ������� ������ � ASCII-���

		//���� ������� ������ ������ � �������� [0-9] ��� [a-z] - ��� ����
		if ((ASCII >= 48 && ASCII <= 57) || (ASCII >=97 && ASCII <= 122)) 
		{
			continue;
		}
		else //����� - ��� ����������
		{
			ErrorString = LANG("Carat.Error.Query.InvalidHash");
			return false;
		}
	}

	//�������� ������������
	ISQuery qSelectAuth(ISDatabase::Instance().GetDB(DBConnectionName), QS_AUTH);
	qSelectAuth.BindValue(":Hash", Hash);
	if (!qSelectAuth.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.SelectLogin").arg(qSelectAuth.GetErrorString());
		return false;
	}

	if (!qSelectAuth.ExecuteFirst())
	{
		ErrorString = LANG("Carat.Error.Query.InvalidLoginOrPassword");
		return false;
	}

	int UserID = qSelectAuth.ReadColumn("usrs_id").toInt();
	bool IsSystem = qSelectAuth.ReadColumn("usrs_issystem").toBool();
	bool IsDeleted = qSelectAuth.ReadColumn("usrs_isdeleted").toBool();
	QString UserFIO = qSelectAuth.ReadColumn("usrs_fio").toString();
	int GroupID = qSelectAuth.ReadColumn("usrs_group").toInt();
	bool GroupFullAccess = qSelectAuth.ReadColumn("usgp_fullaccess").toBool();

	//���� ����� ����� ������� �� ��������
	if (IsDeleted)
	{
		ErrorString = LANG("Carat.Error.Query.LoginIsDeleted");
		return false;
	}

	//���� � ������������ ��� ����� �������
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.LoginNoAccess");
		return false;
	}

	//�������� ������� �������� �� ���������� ������������ � ������
	if (!IsSystem && GroupID == 0)
	{
		ErrorString = LANG("Carat.Error.Query.LoginLinkGroup");
		return false;
	}

	//���� ��� ������������ ��������� ����������� ����� �������� ������� ������
	if (qSelectAuth.ReadColumn("usrs_accountlifetime").toBool())
	{
		QDate DateStart = qSelectAuth.ReadColumn("usrs_accountlifetimestart").toDate();
		QDate DateEnd = qSelectAuth.ReadColumn("usrs_accountlifetimeend").toDate();
		if (DateStart.isValid() && DateEnd.isValid()) //���� ���� ������ � ��������� ����� �������� �����������
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

	//��������� ����������� � ��. ���� �� ������� ������������ - ������
	QString TestConnectionName = ISSystem::GenerateUuid();
	if (!ISDatabase::Instance().Connect(TestConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword))
	{
		Protocol(UserID, CONST_UID_PROTOCOL_BAD_ENTER_APPLICATION, QString(), QString(), QVariant(), ISDatabase::Instance().GetErrorString().simplified());
		ErrorString = LANG("Carat.Error.Query.DatabaseConnection").arg(ISDatabase::Instance().GetErrorString());
		return false;
	}
	Protocol(UserID, CONST_UID_PROTOCOL_ENTER_APPLICATION, QString(), QString(), QVariant(), QString());

	//������������ � �� - ����������� �� �� � ������������� ����� �����������
	ISDatabase::Instance().Disconnect(TestConnectionName);
	TcpMessage->TcpSocket->SetAuthorized(true);
	TcpMessage->TcpSocket->SetUserID(UserID);
	TcpMessage->TcpSocket->SetUserGroupID(GroupID);
	TcpMessage->TcpSocket->SetUserIsSystem(IsSystem);

	//��������� ������ �������.
	bool IsNeedUpdate = false; //�� ��������� ���� ���������� ������ ���� false (����� ������ �������� ���������� ������)
	if (Version.isValid()) //���� ������ �������� ������
	{
		//��������� ��� ������ �� ����������
		QString VersionClientString = Version.toString();
		ISVersion VersionClient(VersionClientString);
		if (VersionClient.IsValid()) //������ �������
		{
			IsNeedUpdate = VersionClient < ISVersionInfo::Instance().Info.Version;
		}
		else //������ ���������
		{
			ISLOGGER_W(__CLASS__, "Client version invalid: " + VersionClientString);
		}
	}

	//����� ���������� � ������������ � ������� �� �������
	TcpAnswer->Parameters["UserID"] = UserID;
	TcpAnswer->Parameters["IsSystem"] = IsSystem;
	TcpAnswer->Parameters["FIO"] = UserFIO;
	TcpAnswer->Parameters["GroupID"] = GroupID;
	TcpAnswer->Parameters["GroupFullAccess"] = GroupFullAccess;
	TcpAnswer->Parameters["IsNeedUpdate"] = IsNeedUpdate;
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
	if (!Ok) //�� ������� �������� �������� � �����
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueInvalid");
		return false;
	}

	if (!TimeoutInt) //���� �������� ����� ���� - ������
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueIsNull");
		return false;
	}

	if (TimeoutInt < 0) //���� �������� ������ ���� - ������
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
	//�������� ��������� ��
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

	//�������� ����� �� �������
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

	//�������� ����������� �����
	QVariantList AccessSpecialList;
	ISQuery qSelectAccessSpecial(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_ACCESS_SPECIAL);
	qSelectAccessSpecial.BindValue(":GroupID", TcpMessage->TcpSocket->GetUserGroupID());
	if (qSelectAccessSpecial.Execute())
	{
		while (qSelectAccessSpecial.Next())
		{
			AccessSpecialList.append(ISUuid(qSelectAccessSpecial.ReadColumn("gast_uid")));
		}
		TcpAnswer->Parameters["AccessSpecial"] = AccessSpecialList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial").arg(qSelectAccessSpecial.GetErrorString());
		return false;
	}

	//�������� ������� � ����������
	QVariantMap SystemSubSystemMap;
	ISQuery qSelectSystems(ISDatabase::Instance().GetDB(DBConnectionName), QS_SYSTEM_SUBSYSTEM);
	qSelectSystems.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qSelectSystems.BindValue(":UserIsSystem", TcpMessage->TcpSocket->GetUserIsSystem());
	if (qSelectSystems.Execute())
	{
		while (qSelectSystems.Next())
		{
			ISUuid SystemUID = qSelectSystems.ReadColumn("stms_uid");
			if (SystemSubSystemMap.contains(SystemUID)) //���� ����� ������� ��� ���� ��������� � Map - ��������� ����������
			{
				QVariantMap SystemMap = SystemSubSystemMap[SystemUID].toMap();
				QVariantList SubSystemList = SystemMap["SubSystems"].toList();
				SubSystemList.append(QVariantMap
				{
					{ "UID", ISUuid(qSelectSystems.ReadColumn("sbsm_uid")) },
					{ "Local", qSelectSystems.ReadColumn("sbsm_localname") },
					{ "Icon", qSelectSystems.ReadColumn("sbsm_icon") },
					{ "Class", qSelectSystems.ReadColumn("sbsm_classname") },
					{ "Table", qSelectSystems.ReadColumn("sbsm_tablename") },
					{ "Hint", qSelectSystems.ReadColumn("sbsm_hint") }
				});
				SystemMap["SubSystems"] = SubSystemList;
				SystemSubSystemMap[SystemUID] = SystemMap;
			}
			else //����� ������� � Map'� ��� ��� - ��������� �
			{
				SystemSubSystemMap[SystemUID] = QVariantMap
				{
					{ "IsSystem", qSelectSystems.ReadColumn("stms_issystem") },
					{ "Local", qSelectSystems.ReadColumn("stms_localname") },
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

	//�������� ������
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
					{ "Table", qSelectPrinting.ReadColumn("rprt_tablename") },
					{ "Local", qSelectPrinting.ReadColumn("rprt_localname") },
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

	//�������� ��������� �������
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

	//�������� �������
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

	//�������� ����������
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

	//�������� ������� �������
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

	//�������� ���������������� ���������
	QVariantMap UserSettingsList;
	ISQuery qSelectUserSetting(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_SETTINGS);
	qSelectUserSetting.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectUserSetting.Execute())
	{
		while (qSelectUserSetting.Next())
		{
			ISUuid GroupUID = qSelectUserSetting.ReadColumn("stgp_uid"),
				SettingUID = qSelectUserSetting.ReadColumn("stgs_uid");
			if (!UserSettingsList.contains(GroupUID)) //���� ����� ������ ��� - ���������
			{
				UserSettingsList[GroupUID] = QVariantMap
				{
					{ "Name", qSelectUserSetting.ReadColumn("stgp_name") },
					{ "Local", qSelectUserSetting.ReadColumn("stgp_localname") },
					{ "Icon", qSelectUserSetting.ReadColumn("stgp_iconname") },
					{ "Hint", qSelectUserSetting.ReadColumn("stgp_hint") },
					{ "Settings", QVariantList() }
				};
			}
			QVariantMap GroupMap = UserSettingsList[GroupUID].toMap(); //�������� ������ �� ��������������
			QVariantList SettingsList = GroupMap["Settings"].toList(); //�������� ������ �������� ������

			//��������� ������ � ������� �� ���������
			QVariantMap Setting =
			{
				{ "UID", SettingUID },
				{ "Name", qSelectUserSetting.ReadColumn("stgs_name") },
				{ "Type", qSelectUserSetting.ReadColumn("stgs_type") },
				{ "WidgetEdit", qSelectUserSetting.ReadColumn("stgs_widgeteditname") },
				{ "Local", qSelectUserSetting.ReadColumn("stgs_localname") },
				{ "Hint", qSelectUserSetting.ReadColumn("stgs_hint") },
				{ "Default", qSelectUserSetting.ReadColumn("stgs_defaultvalue") }
			};

			if (qSelectUserSetting.ReadColumn("count").toInt()) //���� ����� ��������� � ������������ ��� ���� - �������� � ��������
			{
				Setting["Value"] = qSelectUserSetting.ReadColumn("usst_value");
			}
			else //����� ��������� � ������������ ��� - ���������
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

	//�������� ���������
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

	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Test(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);
	Q_UNUSED(TcpAnswer);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

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

	//�������� ��������������� (�� ����) ������ ������ � ��������� ��� �� �������
	QStringList StringList = Dir.entryList(QDir::Files, QDir::Time);
	if (StringList.isEmpty()) //���� ���������� ��� - �������
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//������� ������ ������ � ���� ����� ���������
	QString FilePath;
	for (const QString &FileName : StringList)
	{
		QStringList FileNameList = FileName.split(SYMBOL_POINT);
		if (FileNameList.size() != 4) //���� ��� ����� ���������� - ���������� ���
		{
			continue;
		}
		FilePath = Dir.absolutePath() + '/' + FileName;
		break;
	}

	//���� �� ������ - �������
	if (FilePath.isEmpty())
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//��������� ����
	QFile File(FilePath);
	if (!File.open(QIODevice::ReadOnly)) //�� ������� ������� ����
	{
		ISLOGGER_E(__CLASS__, "Not read file \"" + FilePath + "\": " + File.errorString());
		ErrorString = LANG("Carat.Error.Query.GetLastClient.Other");
		return false;
	}

	//������ � ����� ��������� ����
	QByteArray ByteArray = File.readAll();
	File.close();

	//������������ � base64 � �����
	TcpAnswer->Parameters["Data"] = ByteArray.toBase64();
	return true;
}
//-----------------------------------------------------------------------------
