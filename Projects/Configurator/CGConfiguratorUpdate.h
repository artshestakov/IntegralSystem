#pragma once
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
	void database(); //���������� ���� ������
	void functions(); //���������� �������
	void tables(); //���������� ������
	void systemindexes(); //���������� ��������� ��������
	void indexes(); //���������� ��������
	void compoundindexes(); //���������� ��������� ��������
	void foreigns(); //���������� ������� ������
	void resources(); //���������� ��������
	void classes(); //���������� �������� ������
	void classfields(); //���������� �������� ����� ������
};
//-----------------------------------------------------------------------------
