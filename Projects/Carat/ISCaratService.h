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

protected:
	void start() override;
	void stop() override;
	void processCommand(int Code) override;
	void createApplication(int &argc, char **argv) override;
	int executeApplication() override;
};
//-----------------------------------------------------------------------------
#endif
