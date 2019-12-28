#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaUserData : public QObject
{
	Q_OBJECT

public:
	ISMetaUserData(QObject *parent = 0);
	virtual ~ISMetaUserData();

	bool System; //���������
	int ID; //������������� ������������
	QString Surname; //�������
	QString Name; //���
	QString Patronymic; //��������
	QDate Birthday; //���� ��������
	QString Login; //�����
	QString Password; //������
	QString IPAddress; //IP-������
	QString FullName; //���
	bool AccessAllowed; //������ ��������
	bool AccountLifetime; //������������ ���� �������� ������� ������
	QDate AccountLifetimeStart; //������ �������� ������� ������
	QDate AccountLifetimeEnd; //��������� �������� ������� ������
	bool FixedInactive; //����������� �����������
	int InactiveTimeout; //������� ������������ �����������
	
	int GroupID; //������������� ������ � ������� ������� ������������
	QString GroupName; //������������ ������ ������������
	bool GroupFullAccess; //������ ������
};
//-----------------------------------------------------------------------------
