#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISCBR : public QObject
{
	Q_OBJECT

signals:
	void Answer(const QString &Dollar, const QString &Euro);
	void Error(const QString &ErrorString);

public:
	ISCBR(QObject *parent = 0);
	virtual ~ISCBR();

	void StartQuery(); //������ �������

protected:
	void QueryCBR(); //������
    QString GetValue(const QDomNode &DomNode) const; //�������� �������� ������

private:
	QFutureWatcher<void> *FutureWatcher;

	QString Dollar;
	QString Euro;
};
//-----------------------------------------------------------------------------
