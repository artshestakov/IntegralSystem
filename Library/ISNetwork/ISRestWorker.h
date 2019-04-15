#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISRestWorker : public QObject
{
    Q_OBJECT

signals:
	void Success(const QJsonObject &JsonObject);
	void Error(const QJsonObject &JsonObject);

public:
    static const QString KEY_QNETWORK_REPLY_ERROR;
    static const QString KEY_CONTENT_NOT_FOUND;

    enum class QueryType
	{
        POST,
        GET,
        PATCH,
        DEL
    };

    explicit ISRestWorker(QObject *parent = 0);
    
	void initRequester(const QString& host, int port, QSslConfiguration *value);
    void sendRequest(const QString &apiStr, QueryType type = QueryType::GET, const QVariantMap &data = QVariantMap());
    void sendMulishGetRequest(const QString &apiStr);

    QString GetToken() const;
	void SetToken(const QString &token);

private:
	QByteArray variantMapToJson(QVariantMap data);
	QNetworkRequest createRequest(const QString &apiStr);
	QNetworkReply *sendCustomRequest(QNetworkAccessManager *manager, QNetworkRequest &request, const QString &type, const QVariantMap &data);
	QJsonObject parseReply(QNetworkReply *reply);
	bool onFinishRequest(QNetworkReply *reply);
	void handleQtNetworkErrors(QNetworkReply *reply, QJsonObject &obj);
	QNetworkAccessManager *NetworkAccessManager;

    static const QString HttpTemplate;
    static const QString HttpsTemplate;

    QString Host;
    int Port;
    QString Token;
    QSslConfiguration *SslConfig;
    QString PathTemplate;
};
//-----------------------------------------------------------------------------
