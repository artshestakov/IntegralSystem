#pragma once
#ifndef _CGCONFIGURATORUPDATE_H_INCLUDED
#define _CGCONFIGURATORUPDATE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorUpdate : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorUpdate();
	virtual ~CGConfiguratorUpdate();

public slots:
	bool database(); //���������� ���� ������
	bool functions(); //���������� �������
	bool tables(); //���������� ������
	bool comment(); //��������������� ������
	bool indexesall(); //���������� ���� ��������
	bool systemindexes(); //���������� ��������� ��������
	bool indexes(); //���������� ��������
	bool compoundindexes(); //���������� ��������� ��������
	bool foreigns(); //���������� ������� ������
	bool resources(); //���������� ��������
	bool systemuser(); //���������� ���������� ������������
	bool databasesettings(); //���������� �������� ��
};
//-----------------------------------------------------------------------------
#endif
