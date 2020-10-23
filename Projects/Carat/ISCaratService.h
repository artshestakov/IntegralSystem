#pragma once
#ifndef _ISCARATSERVICE_H_INCLUDED
#define _ISCARATSERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "qtservice.h"
//-----------------------------------------------------------------------------
class ISCaratService : public QtService<QCoreApplication>
{
public:
	ISCaratService(int argc, char **argv);
	virtual ~ISCaratService();

	void Stop()
	{
		stop();
	}

protected:
	void start() override; //������� ��������� ������� �������
	void stop() override; //������� ��������� �������
	void processCommand(int Code) override; //������� �������� �������
	int executeApplication() override; //������� ������������� �������

private:
	int Error(); //��������� ��������� ������� � ���������� ������
};
//-----------------------------------------------------------------------------
#endif
