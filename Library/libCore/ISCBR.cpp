#include "ISCBR.h"
#include "ISDefines.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISCBR::ISCBR(QObject *parent) : QObject(parent)
{
	FutureWatcher = new QFutureWatcher<void>(this);
}
//-----------------------------------------------------------------------------
ISCBR::~ISCBR()
{

}
//-----------------------------------------------------------------------------
void ISCBR::StartQuery()
{
	QFuture<void> Future = QtConcurrent::run(this, &ISCBR::QueryCBR);
	FutureWatcher->setFuture(Future);
}
//-----------------------------------------------------------------------------
void ISCBR::QueryCBR()
{
	QNetworkAccessManager Manager;
	QNetworkRequest Request(QUrl(QString("http://www.cbr.ru/scripts/XML_daily.asp?date_req=%1").arg(DATE_TODAY.toString(DATE_FORMAT_V3))));
	QNetworkReply *Reply = Manager.get(Request);

	QEventLoop EventLoop;
	connect(Reply, &QNetworkReply::finished, &EventLoop, &QEventLoop::quit);
	EventLoop.exec();

	if (Reply->error() == QNetworkReply::NoError)
	{
		QString ReplyString = Reply->readAll();
		QDomElement DomElement = ISSystem::GetDomElement(ReplyString);
		QDomNode DomValute = DomElement.firstChild();
		while (!DomValute.isNull())
		{
			QString ValuteID = DomValute.attributes().namedItem("ID").nodeValue();
			if (ValuteID == "R01235") //Доллар США
			{
				Dollar = GetValue(DomValute.firstChild());
			}
			else if (ValuteID == "R01239") //Евро
			{
				Euro = GetValue(DomValute.firstChild());
			}

			if (Dollar.length() && Euro.length())
			{
				break;
			}

			DomValute = DomValute.nextSibling();
		}

		emit Answer(Dollar, Euro);
	}
	else
	{
		emit Error(Reply->errorString());
	}

	if (Reply)
	{
		delete Reply;
		Reply = nullptr;
	}
}
//-----------------------------------------------------------------------------
QString ISCBR::GetValue(const QDomNode &DomNode) const
{
    QDomNode Temp = DomNode;

    while (!Temp.isNull())
	{
        if (Temp.nodeName() == "Value")
		{
            QString Value = Temp.firstChild().nodeValue();
			Value.replace(SYMBOL_COMMA, '.');
			return Value;
		}

        Temp = Temp.nextSibling();
	}

	return QString();
}
//-----------------------------------------------------------------------------
