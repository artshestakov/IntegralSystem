#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISModelThreadWorker.h"
//-----------------------------------------------------------------------------
class ISModelThreadQuery : public QThread
{
	Q_OBJECT

signals:
	void Started();
	void Finished();
	void ExecuteQuery(const QString &SqlQuery, const QVariantMap &Conditions);
	void Results(const std::vector<QSqlRecord> &Records, const std::vector<QString> &Fields); //����� ����������� �� ������
	void ExecutedQuery(); //������ � ���������� �������
	void ErrorConnection(const QSqlError &SqlError); //������ �� ������ ���������� � ��
	void ErrorQuery(const QSqlError &SqlError, const QString &QueryText); //������ �� ������ � �������

public:
	ISModelThreadQuery(QObject *parent = 0);
	virtual ~ISModelThreadQuery();

	void Execute(const QString &SqlQuery, const QVariantMap &Conditions);

protected:
	void run();

private:
	ISModelThreadWorker *ModelWorker;
};
//-----------------------------------------------------------------------------
