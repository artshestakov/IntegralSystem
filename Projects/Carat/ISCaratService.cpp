#include "ISCaratService.h"
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(int argc, char **argv) : QtService<QCoreApplication>(argc, argv, "Carat")
{
	setServiceDescription("Carat is a back-end service for IntegralSystem.");
}
//-----------------------------------------------------------------------------
ISCaratService::~ISCaratService()
{

}
//-----------------------------------------------------------------------------
void ISCaratService::start()
{
	
}
//-----------------------------------------------------------------------------
void ISCaratService::stop()
{
	QtService<QCoreApplication>::stop();
}
//-----------------------------------------------------------------------------
void ISCaratService::processCommand(int Code)
{
	QtService<QCoreApplication>::processCommand(Code);
}
//-----------------------------------------------------------------------------
void ISCaratService::createApplication(int &argc, char **argv)
{
	QtService<QCoreApplication>::createApplication(argc, argv);
}
//-----------------------------------------------------------------------------
int ISCaratService::executeApplication()
{
	return QtService<QCoreApplication>::executeApplication();
}
//-----------------------------------------------------------------------------
