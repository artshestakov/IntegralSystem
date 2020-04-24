#include "ISSmtpClient.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISSmtpClient::ISSmtpClient(const QString & host, int port, ConnectionType connectionType) :
	socket(nullptr),
    name("localhost"),
    authMethod(AuthPlain),
    connectionTimeout(5000),
    responseTimeout(5000),
    sendMessageTimeout(60000)
{
    setConnectionType(connectionType);

    this->host = host;
    this->port = port;

    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
}
//-----------------------------------------------------------------------------
ISSmtpClient::~ISSmtpClient() 
{
	if (socket)
	{
		delete socket;
	}
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setUser(const QString &user)
{
    this->user = user;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setPassword(const QString &password)
{
    this->password = password;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setAuthMethod(AuthMethod method)
{
    this->authMethod = method;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setHost(const QString &host)
{
    this->host = host;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setPort(int port)
{
    this->port = port;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setConnectionType(ConnectionType ct)
{
    this->connectionType = ct;

	if (socket)
	{
		delete socket;
	}

    switch (connectionType)
    {
    case TcpConnection:
        socket = new QTcpSocket(this);
        break;
    case SslConnection:
    case TlsConnection:
        socket = new QSslSocket(this);
    }
}
//-----------------------------------------------------------------------------
const QString& ISSmtpClient::getErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
const QString& ISSmtpClient::getHost() const
{
    return this->host;
}
//-----------------------------------------------------------------------------
const QString& ISSmtpClient::getUser() const
{
    return this->user;
}
//-----------------------------------------------------------------------------
const QString& ISSmtpClient::getPassword() const
{
    return this->password;
}
//-----------------------------------------------------------------------------
ISSmtpClient::AuthMethod ISSmtpClient::getAuthMethod() const
{
    return this->authMethod;
}
//-----------------------------------------------------------------------------
int ISSmtpClient::getPort() const
{
    return this->port;
}
//-----------------------------------------------------------------------------
ISSmtpClient::ConnectionType ISSmtpClient::getConnectionType() const
{
    return connectionType;
}
//-----------------------------------------------------------------------------
const QString& ISSmtpClient::getName() const
{
    return this->name;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setName(const QString &name)
{
    this->name = name;
}
//-----------------------------------------------------------------------------
const QString & ISSmtpClient::getResponseText() const
{
    return responseText;
}
//-----------------------------------------------------------------------------
int ISSmtpClient::getResponseCode() const
{
    return responseCode;
}
//-----------------------------------------------------------------------------
QTcpSocket* ISSmtpClient::getSocket() 
{
    return socket;
}
//-----------------------------------------------------------------------------
int ISSmtpClient::getConnectionTimeout() const
{
    return connectionTimeout;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setConnectionTimeout(int msec)
{
    connectionTimeout = msec;
}
//-----------------------------------------------------------------------------
int ISSmtpClient::getResponseTimeout() const
{
    return responseTimeout;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setResponseTimeout(int msec)
{
    responseTimeout = msec;
}
//-----------------------------------------------------------------------------
int ISSmtpClient::getSendMessageTimeout() const
{
  return sendMessageTimeout;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::setSendMessageTimeout(int msec)
{
  sendMessageTimeout = msec;
}
//-----------------------------------------------------------------------------
bool ISSmtpClient::connectToHost()
{
    switch (connectionType)
    {
    case TlsConnection:
    case TcpConnection:
        socket->connectToHost(host, port);
        break;
    case SslConnection:
        ((QSslSocket*) socket)->connectToHostEncrypted(host, port);
        break;

    }

    // Tries to connect to server
    if (!socket->waitForConnected(connectionTimeout))
    {
		ErrorString = socket->errorString();
		emit smtpError(ConnectionTimeoutError);
        return false;
    }

    try
    {
        // Wait for the server's response
        waitForResponse();

        // If the response code is not 220 (Service ready)
        // means that is something wrong with the server
        if (responseCode != 220)
        {
            emit smtpError(ServerError);
            return false;
        }

        // Send a EHLO/HELO message to the server
        // The client's first command must be EHLO/HELO
        sendMessage("EHLO " + name);

        // Wait for the server's response
        waitForResponse();

        // The response code needs to be 250.
        if (responseCode != 250) 
		{
            emit smtpError(ServerError);
            return false;
        }

        if (connectionType == TlsConnection) 
		{
            // send a request to start TLS handshake
            sendMessage("STARTTLS");

            // Wait for the server's response
            waitForResponse();

            // The response code needs to be 220.
            if (responseCode != 220) {
                emit smtpError(ServerError);
                return false;
            };

            ((QSslSocket*) socket)->startClientEncryption();

            if (!((QSslSocket*) socket)->waitForEncrypted(connectionTimeout)) 
			{
                ISLOGGER_D(((QSslSocket*) socket)->errorString());
                emit smtpError(ConnectionTimeoutError);
                return false;
            }

            // Send ELHO one more time
            sendMessage("EHLO " + name);

            // Wait for the server's response
            waitForResponse();

            // The response code needs to be 250.
            if (responseCode != 250) 
			{
                emit smtpError(ServerError);
                return false;
            }
        }
    }
    catch (ResponseTimeoutException)
    {
		ErrorString = socket->errorString();
        return false;
    }
    catch (SendMessageTimeoutException)
    {
        return false;
    }

    // If no errors occured the function returns true.
    return true;
}
//-----------------------------------------------------------------------------
bool ISSmtpClient::login()
{
    return login(user, password, authMethod);
}
//-----------------------------------------------------------------------------
bool ISSmtpClient::login(const QString &user, const QString &password, AuthMethod method)
{
    try 
	{
        if (method == AuthPlain)
        {
            // Sending command: AUTH PLAIN base64('\0' + username + '\0' + password)
            sendMessage("AUTH PLAIN " + QByteArray().append((char) 0).append(user).append((char) 0).append(password).toBase64());

            // Wait for the server's response
            waitForResponse();

            // If the response is not 235 then the authentication was faild
            if (responseCode != 235)
            {
                emit smtpError(AuthenticationFailedError);
                return false;
            }
        }
        else if (method == AuthLogin)
        {
            // Sending command: AUTH LOGIN
            sendMessage("AUTH LOGIN");

            // Wait for 334 response code
            waitForResponse();
			if (responseCode != 334)
			{
				emit smtpError(AuthenticationFailedError); 
				return false;
			}

            // Send the username in base64
            sendMessage(QByteArray().append(user).toBase64());

            // Wait for 334
            waitForResponse();
            if (responseCode != 334) 
			{ 
				emit smtpError(AuthenticationFailedError); 
				return false; 
			}

            // Send the password in base64
            sendMessage(QByteArray().append(password).toBase64());

            // Wait for the server's responce
            waitForResponse();

            // If the response is not 235 then the authentication was faild
            if (responseCode != 235)
            {
				ErrorString = "Authentication failed";
                emit smtpError(AuthenticationFailedError);
                return false;
            }
        }
    }
    catch (ResponseTimeoutException)
    {
        // Responce Timeout exceeded
        emit smtpError(AuthenticationFailedError);
        return false;
    }
    catch (SendMessageTimeoutException)
    {
	// Send Timeout exceeded
        emit smtpError(AuthenticationFailedError);
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
bool ISSmtpClient::sendMail(ISMimeMessage& email)
{
    try
    {
        // Send the MAIL command with the sender
        sendMessage("MAIL FROM: <" + email.getSender().getAddress() + '>');

        waitForResponse();

		if (responseCode != 250)
		{
			return false;
		}

        // Send RCPT command for each recipient
        QList<ISEmailAddress*>::const_iterator it, itEnd;
        // To (primary recipients)
        for (it = email.getRecipients().begin(), itEnd = email.getRecipients().end();
             it != itEnd; ++it)
        {
            sendMessage("RCPT TO: <" + (*it)->getAddress() + '>');
            waitForResponse();

			if (responseCode != 250)
			{
				return false;
			}
        }

        // Cc (carbon copy)
        for (it = email.getRecipients(ISMimeMessage::Cc).begin(), itEnd = email.getRecipients(ISMimeMessage::Cc).end();
             it != itEnd; ++it)
        {
            sendMessage("RCPT TO: <" + (*it)->getAddress() + '>');
            waitForResponse();

			if (responseCode != 250)
			{
				return false;
			}
        }

        // Bcc (blind carbon copy)
        for (it = email.getRecipients(ISMimeMessage::Bcc).begin(), itEnd = email.getRecipients(ISMimeMessage::Bcc).end();
             it != itEnd; ++it)
        {
            sendMessage("RCPT TO: <" + (*it)->getAddress() + '>');
            waitForResponse();

			if (responseCode != 250)
			{
				return false;
			}
        }

        // Send DATA command
        sendMessage("DATA");
        waitForResponse();

		if (responseCode != 354)
		{
			return false;
		}

        sendMessage(email.toString());

        // Send \r\n.\r\n to end the mail data
        sendMessage(".");

        waitForResponse();

		if (responseCode != 250)
		{
			return false;
		}
    }
    catch (ResponseTimeoutException)
    {
        return false;
    }
    catch (SendMessageTimeoutException)
    {
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
void ISSmtpClient::quit()
{
    sendMessage("QUIT");
}
//-----------------------------------------------------------------------------
void ISSmtpClient::waitForResponse()
{
    do {
        if (!socket->waitForReadyRead(responseTimeout))
        {
            emit smtpError(ResponseTimeoutError);
            throw ResponseTimeoutException();
        }

        while (socket->canReadLine()) {
            // Save the server's response
            responseText = socket->readLine();

            // Extract the respose code from the server's responce (first 3 digits)
            responseCode = responseText.left(3).toInt();

			if (responseCode / 100 == 4)
			{
				emit smtpError(ServerError);
			}

			if (responseCode / 100 == 5)
			{
				emit smtpError(ClientError);
			}

            if (responseText[3] == ' ') 
			{ 
				return; 
			}
        }
    } while (true);
}
//-----------------------------------------------------------------------------
void ISSmtpClient::sendMessage(const QString &text)
{
    socket->write(text.toUtf8() + "\r\n");
    if (!socket->waitForBytesWritten(sendMessageTimeout))
    {
      emit smtpError(SendDataTimeoutError);
      throw SendMessageTimeoutException();
    }
}
//-----------------------------------------------------------------------------
void ISSmtpClient::socketStateChanged(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
}
//-----------------------------------------------------------------------------
void ISSmtpClient::socketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
	ISLOGGER_E(socket->errorString());
}
//-----------------------------------------------------------------------------
void ISSmtpClient::socketReadyRead()
{

}
//-----------------------------------------------------------------------------
