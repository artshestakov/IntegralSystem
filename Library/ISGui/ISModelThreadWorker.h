#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISModelThreadWorker : public QObject
{
	Q_OBJECT

signals:
	void Started();
	void Finished();
	void Results(const std::vector<QSqlRecord> &Records); //�������� ������
	void ExecutedQuery(); //������ � ���������� �������
	void ErrorConnection(const QSqlError &SqlError); //������ �� ������ ���������� � ��
	void ErrorQuery(const QSqlError &SqlError, const QString &QueryText); //������ �� ������ � �������

public:
	ISModelThreadWorker(QObject *parent = 0);
	virtual ~ISModelThreadWorker();

	void Execute(const QString &SqlQueryText, const QVariantMap &Conditions);
};
//-----------------------------------------------------------------------------
