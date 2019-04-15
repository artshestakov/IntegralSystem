#include "StdAfx.h"
#include "BCContentManager.h"
#include "EXDefines.h"
//-----------------------------------------------------------------------------
BCContentManager::BCContentManager() : QObject()
{
	NetworkAccessManager = new QNetworkAccessManager(this);

	QNetworkRequest Request(QUrl("http://buhzon.ru/media/Index.xml"));
	QNetworkReply *Reply = NetworkAccessManager->get(Request);

	QEventLoop EventLoop;
	connect(Reply, &QNetworkReply::finished, &EventLoop, &QEventLoop::quit);
	EventLoop.exec();

	if (Reply->error() == QNetworkReply::NoError)
	{
		FileIndex = Reply->readAll();
		
		QDomDocument XmlDocument;
		XmlDocument.setContent(FileIndex);
		QDomElement DomElement = XmlDocument.documentElement();
		QDomNode DomNode = DomElement.firstChild();
		while (!DomNode.isNull())
		{
			FileList.append(DomNode.attributes().namedItem("Name").nodeValue());
			DomNode = DomNode.nextSibling();
		}
	}

	if (Reply)
	{
		delete Reply;
		Reply = nullptr;
	}
}
//-----------------------------------------------------------------------------
BCContentManager::~BCContentManager()
{

}
//-----------------------------------------------------------------------------
BCContentManager& BCContentManager::GetInstance()
{
	static BCContentManager ContentManager;
	return ContentManager;
}
//-----------------------------------------------------------------------------
QByteArray BCContentManager::GetFile()
{
	int x = GetRandom(FileList.count() - 1);
	QString FileName = QString("http://buhzon.ru/media/%1").arg(FileList.at(x));

	QFile FileImage(APPLICATION_TEMP_PATH + "/" + FileList.at(x));
	if (FileImage.exists())
	{
		if (FileImage.open(QIODevice::ReadOnly))
		{
			QByteArray ByteArray = FileImage.readAll();
			FileImage.close();
			return ByteArray;
		}
	}

	QNetworkRequest Request(QUrl(FileName.toUtf8()));
	QNetworkReply *Reply = NetworkAccessManager->get(Request);

	QEventLoop EventLoop;
	connect(Reply, &QNetworkReply::finished, &EventLoop, &QEventLoop::quit);
	EventLoop.exec();

	if (Reply->error() == QNetworkReply::NoError)
	{
		LastExtension = QFileInfo(FileName).suffix();
		QByteArray BytwArray = Reply->readAll();

		if (FileImage.open(QIODevice::WriteOnly))
		{
			FileImage.write(BytwArray);
			FileImage.close();
		}

		return BytwArray;
	}
	else
	{
		qDebug() << Reply->errorString();
	}

	if (Reply)
	{
		delete Reply;
		Reply = nullptr;
	}

	return QByteArray();
}
//-----------------------------------------------------------------------------
QString BCContentManager::GetExtension() const
{
	return LastExtension;
}
//-----------------------------------------------------------------------------
int BCContentManager::GetRandom(int Max)
{
	srand(time(NULL));
	int Result = 0;
	while (true)
	{
		Result = rand();
		if (Result > Max)
		{
			continue;
		}
		else
		{
			return Result;
		}
	}
}
//-----------------------------------------------------------------------------
