#include "ISTcpServerWorker.h"
#include "ISTcp.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE = PREPARE_QUERY("SELECT clsz_tablename, clsz_fieldname, clsz_size "
											  "FROM _columnsize "
											  "WHERE clsz_user = currentuserid()");
//-----------------------------------------------------------------------------
ISTcpServerWorker::ISTcpServerWorker(QObject *parent)
	: ISTcpServerBase(parent),
	TcpSocket(nullptr),
	BufferSize(0)
{
	Functions[API_TEST_QUERY] = std::mem_fn(&ISTcpServerWorker::TestQuery);
	Functions[API_SLEEP] = std::mem_fn(&ISTcpServerWorker::Sleep);
	Functions[API_COLUMN_SIZER] = std::mem_fn(&ISTcpServerWorker::ColumnSizer);
}
//-----------------------------------------------------------------------------
ISTcpServerWorker::~ISTcpServerWorker()
{
	
}
//-----------------------------------------------------------------------------
bool ISTcpServerWorker::Run(quint16 Port)
{
	bool Result = ISTcpServerBase::Run(Port);
	if (Result) //������� ����� ����������
	{
		//�������� ������������ � ������ � �������� ��� ������ ������� ���������
		QLocalSocket LocalSocket;
		LocalSocket.connectToServer(CARAT_CONTROLLER_PORT, QIODevice::WriteOnly);
		Result = LocalSocket.waitForConnected();
		if (Result) //����������� � ������ ���� - �����������
		{
			LocalSocket.disconnectFromServer();
		}
		else //������������ � ������ �� ������� - ������
		{
			SetErrorString("Error connecting to carat: " + LocalSocket.errorString());
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::incomingConnection(qintptr SocketDescriptor)
{
	ISTcpServerBase::incomingConnection(SocketDescriptor);
	if (TcpSocket) //���� ��� ���� �������� ����������� - ����������� ����� � �������
	{
		QTcpSocket *SocketExcess = nextPendingConnection();
		SendError(SocketExcess, "The port is already in use by another connection");
		SocketExcess->deleteLater();
	}
	else //��������� ����������� ��� - ��������� ���
	{
		TcpSocket = nextPendingConnection();
		connect(TcpSocket, &QTcpSocket::readyRead, this, &ISTcpServerWorker::ReadyRead);
		connect(TcpSocket, &QTcpSocket::disconnected, this, &ISTcpServerWorker::Disconnected);
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::ReadyRead()
{
	Buffer.append(TcpSocket->readAll());

	//���� ������ ������� ��� ���������� - �������� ���
	if (!BufferSize)
	{
		BufferSize = ISTcp::GetQuerySizeFromBuffer(Buffer);
	}

	//������ ������
	ISTcpAnswer TcpAnswer;
	if (BufferSize && !Buffer.isEmpty()) //������ ������� ������� - ��� � �������
	{
		//���� ������ ������ �� ���� - ������� �� �������
		if (Buffer.size() != BufferSize)
		{
			return;
		}

		//�������� ���������� �������
		QVariantMap VariantMap;
		QString ErrorString;
		if (ISTcp::IsValidQuery(Buffer, VariantMap, ErrorString))
		{
			QString QueryType = VariantMap["Type"].toString();
			if (Functions.count(QueryType)) //���� ����� ������ ���������� - ��������� ���
			{
				Functions[QueryType](*this, VariantMap["Parameters"].toMap(), TcpAnswer);
			}
			else //������ �� ���������� - ���������� ������
			{
				TcpAnswer.SetError(QString("Unknown query \"%1\"").arg(QueryType));
			}
		}
		else
		{
			TcpAnswer.SetError(ErrorString);
		}
	}
	else //�� ������� ������� ������ ������� - ���������� ������
	{
		TcpAnswer.SetError("Query is not a valid");
	}
	Send(TcpSocket, TcpAnswer);

	//������� ����� � ��� ������
	Buffer.clear();
	BufferSize = 0;
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::Disconnected()
{
	TcpSocket->deleteLater();
	close();
	QCoreApplication::quit();
}
//-----------------------------------------------------------------------------
bool ISTcpServerWorker::CheckField(const QVariantMap &Parameters, const ISVectorString &Fields, ISTcpAnswer &TcpAnswer)
{
	for (const QString &FieldName : Fields)
	{
		if (!Parameters.contains(FieldName))
		{
			TcpAnswer.SetError(QString("Not found field \"%1\"").arg(FieldName));
			return false;
		}

		if (!Parameters[FieldName].isValid())
		{
			TcpAnswer.SetError(QString("Field \"%1\" is empty").arg(FieldName));
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::TestQuery(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
    Q_UNUSED(Parameters);
	TcpAnswer["DateTime"] = QDateTime::currentDateTime().toString(FORMAT_DATE_TIME_V2);
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::Sleep(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
	if (!CheckField(Parameters, { "MSec" }, TcpAnswer))
	{
		return;
	}

	bool Result = true;
	int MSec = Parameters["MSec"].toInt(&Result);
	if (Result)
	{
		Result = MSec > 0;
	}

	if (Result)
	{
		ISSleep(MSec);
	}
	else
	{
		TcpAnswer.SetError("Invalid value msec");
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::ColumnSizer(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
	ISQuery qSelect(QS_COLUMN_SIZE);
	if (qSelect.Execute())
	{
		QVariantList Tables;
		while (qSelect.Next())
		{
			Tables.push_back(QVariantMap
			{
				{ "TableName", qSelect.ReadColumn("clsz_tablename") },
				{ "FieldName", qSelect.ReadColumn("clsz_fieldname") },
				{ "Size", qSelect.ReadColumn("clsz_size") }
			});
		}
		TcpAnswer["Tables"] = Tables;
	}
	else
	{
		TcpAnswer.SetError(qSelect.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
