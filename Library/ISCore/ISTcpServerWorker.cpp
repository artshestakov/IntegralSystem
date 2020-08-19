#include "ISTcpServerWorker.h"
#include "ISTcp.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISQuery.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISNetwork.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE = PREPARE_QUERY("SELECT clsz_tablename, clsz_fieldname, clsz_size "
											  "FROM _columnsize "
											  "WHERE clsz_user = currentuserid()");
//-----------------------------------------------------------------------------
static QString QS_PARAGRAPHS = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname, prhs_default "
											 "FROM _paragraphs "
											 "WHERE NOT prhs_isdeleted "
											 "ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
static QString QS_SORTING_COLUMN = PREPARE_QUERY("SELECT sgts_tablename, sgts_fieldname, sgts_sorting "
												 "FROM _sortingtables "
												 "WHERE NOT sgts_isdeleted "
												 "AND sgts_user = currentuserid()");
//-----------------------------------------------------------------------------
ISTcpServerWorker::ISTcpServerWorker(QObject *parent)
	: ISTcpServerBase(parent),
	TcpSocket(nullptr),
	BufferSize(0)
{
	Functions[API_TEST_QUERY] = std::mem_fn(&ISTcpServerWorker::TestQuery);
	Functions[API_SLEEP] = std::mem_fn(&ISTcpServerWorker::Sleep);
	Functions[API_GET_META_DATA] = std::mem_fn(&ISTcpServerWorker::GetMetaData);
	Functions[API_FUNCTION_LIST] = std::mem_fn(&ISTcpServerWorker::FunctionList);
}
//-----------------------------------------------------------------------------
ISTcpServerWorker::~ISTcpServerWorker()
{
	
}
//-----------------------------------------------------------------------------
bool ISTcpServerWorker::Run(quint16 Port)
{
	bool Result = ISTcpServerBase::Run(Port);
	if (Result) //Базовый сервер запустился
	{
		//Пытаемся подключиться к карату и сообщить что запуск воркера произошёл
		QLocalSocket LocalSocket;
		LocalSocket.connectToServer(CARAT_CONTROLLER_PORT, QIODevice::WriteOnly);
		Result = LocalSocket.waitForConnected();
		if (Result) //Подключение к карату есть - отключаемся
		{
			LocalSocket.disconnectFromServer();
		}
		else //Подключиться к карату не удалось - выкатываем предупреждение и за ошибку не считаем
		{
			ISLOGGER_W("Connecting to carat: " + LocalSocket.errorString());
			Result = true;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::incomingConnection(qintptr SocketDescriptor)
{
	ISTcpServerBase::incomingConnection(SocketDescriptor);
	if (TcpSocket) //Если уже есть активное подключение - выбрасываем новое с ошибкой
	{
		QTcpSocket *SocketExcess = nextPendingConnection();
		SendError(SocketExcess, "The port is already in use by another connection");
		SocketExcess->deleteLater();
	}
	else //Активного подключения нет - принимаем его
	{
		TcpSocket = nextPendingConnection();
		connect(TcpSocket, &QTcpSocket::readyRead, this, &ISTcpServerWorker::ReadyRead);
		connect(TcpSocket, &QTcpSocket::disconnected, this, &ISTcpServerWorker::Disconnected);
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::Disconnected()
{
	ISLOGGER_I("Disconnected " + ISNetwork().ParseIPAddress(TcpSocket->peerAddress().toString()));
	TcpSocket->deleteLater();
	close();
	ISCore::ExitApplication();
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::ReadyRead()
{
	Buffer.append(TcpSocket->readAll());
	if (!BufferSize) //Если размер запроса ещё неизвестен - получаем его
	{
		BufferSize = ISTcp::GetQuerySizeFromBuffer(Buffer);
	}

	//Объект ответа
	ISTcpAnswer TcpAnswer;
	if (BufferSize && !Buffer.isEmpty()) //Размер запроса получен - все в порядке
	{
		if (Buffer.size() != BufferSize) //Если запрос пришёл не весь - выходим из функции
		{
			return;
		}

		//Проверка валидности запроса
		QVariantMap VariantMap;
		QString error_string;
		if (ISTcp::IsValidQuery(Buffer, VariantMap, error_string))
		{
			QString QueryType = VariantMap["Type"].toString();
			if (Functions.count(QueryType)) //Если такой запрос существует - выполняем его
			{
				ISLOGGER_I("Incoming query \"" + QueryType + "\"");
				Functions[QueryType](*this, VariantMap["Parameters"].toMap(), TcpAnswer);
			}
			else //Запрос не существует - отправляем ошибку
			{
				TcpAnswer.SetError(QString("Unknown query \"%1\"").arg(QueryType));
				ISLOGGER_E(QString("Unknown query \"%1\"").arg(QueryType));
			}
		}
		else
		{
			TcpAnswer.SetError(error_string);
			ISLOGGER_E(error_string);
		}
	}
	else //Не удалось извлечь размер запроса - возвращаем ошибку
	{
		TcpAnswer.SetError("Query is not a valid");
		ISLOGGER_E("Query is not a valid");
	}
	Send(TcpSocket, TcpAnswer);

	//Очищаем буфер и его размер
	Buffer.clear();
	BufferSize = 0;
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
    Q_UNUSED(TcpAnswer);
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
void ISTcpServerWorker::GetMetaData(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
    Q_UNUSED(Parameters);

	//Получаем размеры полей
	ISQuery qSelectColumnSize(QS_COLUMN_SIZE);
	if (qSelectColumnSize.Execute())
	{
		QVariantList Tables;
		while (qSelectColumnSize.Next())
		{
			Tables.push_back(QVariantMap
			{
				{ "TableName", qSelectColumnSize.ReadColumn("clsz_tablename") },
				{ "FieldName", qSelectColumnSize.ReadColumn("clsz_fieldname") },
				{ "Size", qSelectColumnSize.ReadColumn("clsz_size") }
			});
		}
		TcpAnswer["Tables"] = Tables;
	}
	else
	{
		TcpAnswer.SetError("Error getting column size: " + qSelectColumnSize.GetErrorString());
		return;
	}

	//Получаем параграфы
	ISQuery qSelectParagraph(QS_PARAGRAPHS);
	if (qSelectParagraph.Execute())
	{
		QVariantList Paragraphs;
		while (qSelectParagraph.Next())
		{
			Paragraphs.push_back(QVariantMap
			{
				{ "UID", qSelectParagraph.ReadColumn("prhs_uid") },
				{ "Name", qSelectParagraph.ReadColumn("prhs_name") },
				{ "LocalName", qSelectParagraph.ReadColumn("prhs_localname") },
				{ "ToolTip", qSelectParagraph.ReadColumn("prhs_tooltip") },
				{ "Icon", qSelectParagraph.ReadColumn("prhs_icon") },
				{ "ClassName", qSelectParagraph.ReadColumn("prhs_classname") },
				{ "Default", qSelectParagraph.ReadColumn("prhs_default") }
			});
		}
		TcpAnswer["Paragraphs"] = Paragraphs;
	}
	else
	{
		TcpAnswer.SetError("Error getting paragraphs: " + qSelectParagraph.GetErrorString());
		return;
	}

	//Получаем сортировки
	ISQuery qSelectSortingColumn(QS_SORTING_COLUMN);
	if (qSelectSortingColumn.Execute())
	{
		QVariantList SortingColumns;
		while (qSelectSortingColumn.Next())
		{
			SortingColumns.push_back(QVariantMap
			{
				{ "TableName", qSelectSortingColumn.ReadColumn("sgts_tablename") },
				{ "FieldName", qSelectSortingColumn.ReadColumn("sgts_fieldname") },
				{ "Sorting", qSelectSortingColumn.ReadColumn("sgts_sorting") }
			});
		}
		TcpAnswer["SortingColumns"] = SortingColumns;
	}
	else
	{
		TcpAnswer.SetError("Error getting sorting columns: " + qSelectSortingColumn.GetErrorString());
		return;
	}
}
//-----------------------------------------------------------------------------
void ISTcpServerWorker::FunctionList(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
    Q_UNUSED(Parameters);

	QVariantList VariantList;
	for (const auto &MapItem : Functions)
	{
		VariantList.push_back(MapItem.first);
	}
	TcpAnswer["Functions"] = VariantList;
}
//-----------------------------------------------------------------------------
