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
static QString QS_AUTH = PREPARE_QUERY("SELECT "
									   "usrs_issystem, "
									   "usrs_isdeleted, "
									   "usrs_group, "
									   "usrs_accessallowed, "
									   "usrs_accountlifetime, "
									   "usrs_accountlifetimestart, "
									   "usrs_accountlifetimeend "
									   "FROM _users "
									   "WHERE usrs_login = :Login");
//-----------------------------------------------------------------------------
static QString QS_SYSTEMS = PREPARE_QUERY("SELECT "
										  "stms_issystem, stms_id, stms_uid, stms_localname, stms_orderid, stms_icon, stms_hint, "
										  "sbsm_id, sbsm_uid, sbsm_localname, sbsm_orderid, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
										  "FROM _subsystems "
										  "LEFT JOIN _systems ON stms_uid = sbsm_system "
										  "WHERE NOT sbsm_isdeleted "
										  "ORDER BY stms_orderid, sbsm_orderid");
//-----------------------------------------------------------------------------
ISTcpWorker::ISTcpWorker(const QString &db_host, int db_port, const QString &db_name, const QString &db_user, const QString &db_password)
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	DBHost(db_host),
	DBPort(db_port),
	DBName(db_name),
	DBUser(db_user),
	DBPassword(db_password),
	IsRunning(false),
	CurrentMessage(nullptr)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpWorker::~ISTcpWorker()
{
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
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
	IsRunning = true;
	CurrentMessage = TcpMessage;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Run()
{
	//��������� ��� ����������� � ��
	DBConnectionName = QString::number((unsigned long)QThread::currentThreadId());

	//�������� ������������ � ��
	if (!ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword))
	{
		ISLOGGER_E("Not connected to database: " + ISDatabase::Instance().GetErrorString());
		emit Started();
		return;
	}

	ISLOGGER_I("Started worker with thread id: " + DBConnectionName);
	emit Started();
	while (true)
	{
		//�������� �� ���� ����������� � ��� ���������� ������
		ISSleep(1);

		//�������� ������� ���������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		ISTcpMessage *tcp_message = CurrentMessage;
		CurrentMessage = nullptr;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//���� ��������� ��������� �� ��������� - ��������� ���
		if (tcp_message)
		{
			bool Result = false;
			unsigned __int64 PerfomanceMsec = 0;
			ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpSocket);

			if (tcp_message->IsValid()) //���� ��������� �������� - ��������� � ����������
			{
				PerfomanceMsec = ISAlgorithm::GetTick(); //���������� ������� �����
				switch (tcp_message->Type)
				{
				case ISNamespace::AMT_Auth: Result = Auth(tcp_message, TcpAnswer); break;
				case ISNamespace::AMT_Sleep: Result = Sleep(tcp_message, TcpAnswer); break;
				case ISNamespace::AMT_GetMetaData: Result = GetMetaData(tcp_message, TcpAnswer); break;
				}
				PerfomanceMsec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), PerfomanceMsec);
			}
			else //��������� �� ��������
			{
				ErrorString = tcp_message->GetErrorString();
			}

			//��������� ���-���������
			QString LogText = QString("%1 message \"%2\". Size: %3 Chunk: %4 Parse msec: %5 MSec: %6").
				arg(Result ? "Done" : "Failed").
				arg(tcp_message->TypeName).
				arg(tcp_message->Size).
				arg(tcp_message->ChunkCount).
				arg(tcp_message->ParseMSec).
				arg(PerfomanceMsec);

			if (!Result) //������ �������� � ������� - ������������� ����� ������ � ������
			{
				TcpAnswer->SetError(ErrorString);
				LogText.append("\nError string: " + ErrorString);
			}

			//����������, ��������� ����� � ������� ������� � ��������� ������
			Result ? ISLOGGER_I(LogText) : ISLOGGER_E(LogText);
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

	//���������, �� ����������� �� ��� ������. ���� ����������� - ������� � �������
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

	//�������� ������������
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

	//���� ����� ����� ������� �� ��������
	if (qSelectAuth.ReadColumn("usrs_isdeleted").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.LoginIsDeleted").arg(LoginString);
		return false;
	}

	//���� � ������������ ��� ����� �������
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.LoginNoAccess");
		return false;
	}

	//�������� ������� �������� ������������ � ������
	if (!qSelectAuth.ReadColumn("usrs_issystem").toBool()) //���� ������������ �� ��������� - ��������� ��������
	{
		if (qSelectAuth.ReadColumn("usrs_group").toInt() == 0) //�������� �����������
		{
			ErrorString = LANG("Carat.Error.Query.LoginLinkGroup");
			return false;
		}
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

	//��������� ����������� � ��
	QString TestConnectionName = ISSystem::GenerateUuid();
	if (!ISDatabase::Instance().Connect(TestConnectionName, DBHost, DBPort, DBName, LoginString, PasswordString))
	{
		ErrorString = LANG("Carat.Error.Query.DatabaseConnection").arg(ISDatabase::Instance().GetErrorString());
		return false;
	}
	ISDatabase::Instance().Disconnect(TestConnectionName);
	TcpMessage->TcpSocket->SetAuthorized(true);
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
	QVariant Login = CheckNullField("Login", TcpMessage->Parameters);
	if (!Login.isValid())
	{
		return false;
	}
	QString LoginString = Login.toString();

	QVariantList SystemsSubSystems;

	//�������� ������� � ����������
	ISQuery qSelectSystems(ISDatabase::Instance().GetDB(DBConnectionName), QS_SYSTEMS);
	if (qSelectSystems.Execute())
	{
		while (qSelectSystems.Next())
		{
			SystemsSubSystems.append(QVariantMap
			{
				{ "stms_issystem", qSelectSystems.ReadColumn("stms_issystem") },
				{ "stms_id", qSelectSystems.ReadColumn("stms_id") },
				{ "stms_uid", qSelectSystems.ReadColumn("stms_uid") },
				{ "stms_localname", qSelectSystems.ReadColumn("stms_localname") },
				{ "stms_orderid", qSelectSystems.ReadColumn("stms_orderid") },
				{ "stms_icon", qSelectSystems.ReadColumn("stms_icon") },
				{ "stms_hint", qSelectSystems.ReadColumn("stms_hint").toByteArray() },
				{ "sbsm_id", qSelectSystems.ReadColumn("sbsm_id") },
				{ "sbsm_uid", qSelectSystems.ReadColumn("sbsm_uid") },
				{ "sbsm_localname", qSelectSystems.ReadColumn("sbsm_localname").toByteArray() },
				{ "sbsm_orderid", qSelectSystems.ReadColumn("sbsm_orderid") },
				{ "sbsm_icon", qSelectSystems.ReadColumn("sbsm_icon") },
				{ "sbsm_classname", qSelectSystems.ReadColumn("sbsm_classname") },
				{ "sbsm_tablename", qSelectSystems.ReadColumn("sbsm_tablename") },
				{ "sbsm_hint", qSelectSystems.ReadColumn("sbsm_hint").toByteArray() }
			});
		}
		TcpAnswer->Parameters.insert("SystemsSubSystems", SystemsSubSystems);
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.InitializeMetaSystems").arg(qSelectSystems.GetErrorString());
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
