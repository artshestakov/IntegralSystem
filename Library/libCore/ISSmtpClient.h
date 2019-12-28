#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISMimeMessage.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISSmtpClient : public QObject
{
    Q_OBJECT

public:
    enum AuthMethod
    {
        AuthPlain,
        AuthLogin
    };

    enum SmtpError
    {
        ConnectionTimeoutError,
        ResponseTimeoutError,
        SendDataTimeoutError,
        AuthenticationFailedError,
        ServerError,    // 4xx smtp error
        ClientError     // 5xx smtp error
    };

    enum ConnectionType
    {
        TcpConnection,
        SslConnection,
        TlsConnection       // STARTTLS
    };

    ISSmtpClient(const QString & host = "localhost", int port = 25, ConnectionType ct = TcpConnection);
    ~ISSmtpClient();

	const QString& getErrorString() const;

    const QString& getHost() const;
    void setHost(const QString &host);

    int getPort() const;
    void setPort(int port);

    const QString& getName() const;
    void setName(const QString &name);

    ConnectionType getConnectionType() const;
    void setConnectionType(ConnectionType ct);

    const QString & getUser() const;
    void setUser(const QString &user);

    const QString & getPassword() const;
    void setPassword(const QString &password);

    ISSmtpClient::AuthMethod getAuthMethod() const;
    void setAuthMethod(AuthMethod method);

    const QString & getResponseText() const;
    int getResponseCode() const;

    int getConnectionTimeout() const;
    void setConnectionTimeout(int msec);

    int getResponseTimeout() const;
    void setResponseTimeout(int msec);
    
    int getSendMessageTimeout() const;
    void setSendMessageTimeout(int msec);

    QTcpSocket* getSocket();

    bool connectToHost();

    bool login();
    bool login(const QString &user, const QString &password, AuthMethod method = AuthLogin);

    bool sendMail(ISMimeMessage& email);

    void quit();

protected:
	void waitForResponse();
	void sendMessage(const QString &text);

protected slots:
	void socketStateChanged(QAbstractSocket::SocketState state);
	void socketError(QAbstractSocket::SocketError error);
	void socketReadyRead();

protected:
    QTcpSocket *socket;

    QString host;
    int port;
    ConnectionType connectionType;
    QString name;

    QString user;
    QString password;
    AuthMethod authMethod;

    int connectionTimeout;
    int responseTimeout;
    int sendMessageTimeout;
    
    QString responseText;
    int responseCode;

    class ResponseTimeoutException {};
    class SendMessageTimeoutException {};

signals:
	void smtpError(ISSmtpClient::SmtpError e);

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
