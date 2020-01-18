#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
struct ISMetaUserData
{
	ISMetaUserData();

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
