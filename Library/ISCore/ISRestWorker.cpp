#include "ISRestWorker.h"
//-----------------------------------------------------------------------------
const QString ISRestWorker::HttpTemplate = "http://%1:%2/api/%3";
const QString ISRestWorker::HttpsTemplate = "https://%1:%2/api/%3";
const QString ISRestWorker::KEY_QNETWORK_REPLY_ERROR = "QNetworkReplyError";
const QString ISRestWorker::KEY_CONTENT_NOT_FOUND = "ContentNotFoundError";
//-----------------------------------------------------------------------------
ISRestWorker::ISRestWorker(QObject *parent) : QObject(parent)
{
	NetworkAccessManager = new QNetworkAccessManager(this);
}
//-----------------------------------------------------------------------------
void ISRestWorker::initRequester(const QString &host, int port, QSslConfiguration *value)
{
	Host = host;
	Port = port;
	SslConfig = value;
	if (SslConfig != nullptr)
	{
		PathTemplate = HttpsTemplate;
	}
	else
	{
		PathTemplate = HttpTemplate;
	}
}
//-----------------------------------------------------------------------------
void ISRestWorker::sendRequest(const QString &apiStr, ISRestWorker::QueryType type, const QVariantMap &data)
{
	QNetworkRequest request = createRequest(apiStr);

	QNetworkReply *reply = nullptr;
	switch (type)
	{
	case QueryType::POST:
	{
		QByteArray postDataByteArray = variantMapToJson(data);
		reply = NetworkAccessManager->post(request, postDataByteArray);
		break;
	}
	case QueryType::GET:
	{
		reply = NetworkAccessManager->get(request);
		break;
	}
	case QueryType::DEL:
	{
		if (data.isEmpty())
		{
			reply = NetworkAccessManager->deleteResource(request);
		}
		else
		{
			reply = sendCustomRequest(NetworkAccessManager, request, "DELETE", data);
		}
		break;
	}
	case QueryType::PATCH:
	{
		reply = sendCustomRequest(NetworkAccessManager, request, "PATCH", data);
		break;
	}
	default: reply = nullptr; Q_ASSERT(false);
	}

	connect(reply, &QNetworkReply::finished, [=]
	{
		QJsonObject JsonObject = parseReply(reply);

		if (onFinishRequest(reply))
		{
			emit Success(JsonObject);
		}
		else
		{
			emit Error(JsonObject);
			handleQtNetworkErrors(reply, JsonObject);
		}

		reply->close();
		reply->deleteLater();
	});

}
//-----------------------------------------------------------------------------
void ISRestWorker::sendMulishGetRequest(const QString &apiStr)
{
	QNetworkRequest request = createRequest(apiStr);
	qInfo() << "GET REQUEST " << request.url().toString() << "\n";
	auto reply = NetworkAccessManager->get(request);

	connect(reply, &QNetworkReply::finished, [=]
	{
		QJsonObject obj = parseReply(reply);
		if (onFinishRequest(reply))
		{
			emit Success(obj);

			QString nextPage = obj.value("next").toString();
			if (!nextPage.isEmpty())
			{
				QStringList apiMethodWithPage = nextPage.split("api/");
				sendMulishGetRequest(apiMethodWithPage.value(1));
			}
		}
		else
		{
			handleQtNetworkErrors(reply, obj);
			emit Error(obj);
		}
		reply->close();
		reply->deleteLater();
	});
}
//-----------------------------------------------------------------------------
QString ISRestWorker::GetToken() const
{
	return Token;
}
//-----------------------------------------------------------------------------
void ISRestWorker::SetToken(const QString &token)
{
	Token = token;
}
//-----------------------------------------------------------------------------
QByteArray ISRestWorker::variantMapToJson(QVariantMap data)
{
	QJsonDocument postDataDoc = QJsonDocument::fromVariant(data);
	QByteArray postDataByteArray = postDataDoc.toJson();

	return postDataByteArray;
}
//-----------------------------------------------------------------------------
QNetworkRequest ISRestWorker::createRequest(const QString &apiStr)
{
	QNetworkRequest request;
	QString url = apiStr;
	request.setUrl(QUrl(url));
	request.setRawHeader("Content-Type", "application/json");
	if (!Token.isEmpty())
	{
		request.setRawHeader("Authorization", QString("token %1").arg(Token).toUtf8());
	}

	if (SslConfig != nullptr)
	{
		request.setSslConfiguration(*SslConfig);
	}

	return request;
}
//-----------------------------------------------------------------------------
QNetworkReply* ISRestWorker::sendCustomRequest(QNetworkAccessManager* manager, QNetworkRequest &request, const QString &type, const QVariantMap &data)
{
	request.setRawHeader("HTTP", type.toUtf8());
	QByteArray postDataByteArray = variantMapToJson(data);
	QBuffer *buff = new QBuffer;
	buff->setData(postDataByteArray);
	buff->open(QIODevice::ReadOnly);
	QNetworkReply* reply = manager->sendCustomRequest(request, type.toUtf8(), buff);
	buff->setParent(reply);
	return reply;
}
//-----------------------------------------------------------------------------
QJsonObject ISRestWorker::parseReply(QNetworkReply *reply)
{
	QJsonObject jsonObj;
	QJsonDocument jsonDoc;
	QJsonParseError parseError;
	auto replyText = reply->readAll();
	jsonDoc = QJsonDocument::fromJson(replyText, &parseError);
	if (parseError.error != QJsonParseError::NoError)
	{
		qDebug() << replyText;
		qWarning() << "Json parse error: " << parseError.errorString();
	}
	else
	{
		if (jsonDoc.isObject())
		{
			jsonObj = jsonDoc.object();
		}
		else if (jsonDoc.isArray())
		{
			jsonObj["non_field_errors"] = jsonDoc.array();
		}
	}
	return jsonObj;
}
//-----------------------------------------------------------------------------
bool ISRestWorker::onFinishRequest(QNetworkReply *reply)
{
	auto replyError = reply->error();
	if (replyError == QNetworkReply::NoError)
	{
		int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if ((code >= 200) && (code < 300))
		{
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
void ISRestWorker::handleQtNetworkErrors(QNetworkReply *reply, QJsonObject &obj)
{
	auto replyError = reply->error();
	if (!(replyError == QNetworkReply::NoError ||
		replyError == QNetworkReply::ContentNotFoundError ||
		replyError == QNetworkReply::ContentAccessDenied ||
		replyError == QNetworkReply::ProtocolInvalidOperationError
		)) {
		qDebug() << reply->error();
		obj[KEY_QNETWORK_REPLY_ERROR] = reply->errorString();
	}
	else if (replyError == QNetworkReply::ContentNotFoundError)
		obj[KEY_CONTENT_NOT_FOUND] = reply->errorString();
}
//-----------------------------------------------------------------------------
