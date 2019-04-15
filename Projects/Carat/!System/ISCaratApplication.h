#pragma once
//-----------------------------------------------------------------------------
#include "QtService.h"
#include "ISCaratService.h"
//-----------------------------------------------------------------------------
class ISCaratApplication : public QtService<QCoreApplication>
{
public:
	ISCaratApplication(int argc, char **argv);
	virtual ~ISCaratApplication();

	bool ConnectToDB() const; //Подключение к БД

protected:
	void start() override;
	void pause() override;
	void resume() override;
	void stop() override;

private:
	ISCaratService *Service;
};
//-----------------------------------------------------------------------------
