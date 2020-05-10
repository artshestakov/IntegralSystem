#include "ISTcpServerWorker.h"
#include "ISTcp.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpServerWorker::ISTcpServerWorker(QObject *parent)
	: ISTcpServerBase(parent),
	TcpSocket(nullptr),
	BufferSize(0)
{
	Functions[API_TEST_QUERY] = std::mem_fn(&ISTcpServerWorker::TestQuery);
	Functions[API_SLEEP] = std::mem_fn(&ISTcpServerWorker::Sleep);
}
//-----------------------------------------------------------------------------
ISTcpServerWorker::~ISTcpServerWorker()
{
	
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
			if (!Functions.count(QueryType) == 1) //���� ����� ������ ����������
			{
				Functions[QueryType](*this, VariantMap["Parameters"].toMap(), TcpAnswer);
			}
			else
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
	Buffer.clear();
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
