#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratService.h"
//-----------------------------------------------------------------------------
class ISCaratApplication : public QCoreApplication
{
public:
	ISCaratApplication(int argc, char **argv);
	virtual ~ISCaratApplication();

	bool ConnectToDB() const; //Подключение к БД
	void Start();

private:
	ISCaratService *Service;
};
//-----------------------------------------------------------------------------
