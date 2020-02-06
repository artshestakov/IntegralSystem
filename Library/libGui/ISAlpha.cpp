#include "ISAlpha.h"
//-----------------------------------------------------------------------------
#include "ISDeviceObjectForm.h"
#include "ISDeviceListForm.h"
#include "ISDeviceSubSystem.h"
#include "ISClientsSqlModel.h"
//-----------------------------------------------------------------------------
ISAlpha::ISAlpha(QObject *parent) : ISObjectInterface(parent)
{

}
//-----------------------------------------------------------------------------
ISAlpha::~ISAlpha()
{

}
//-----------------------------------------------------------------------------
void ISAlpha::RegisterMetaTypes() const
{
	qRegisterMetaType<ISDeviceObjectForm*>("ISDeviceObjectForm");
	qRegisterMetaType<ISDeviceListForm*>("ISDeviceListForm");
	qRegisterMetaType<ISDeviceSubSystem*>("ISDeviceSubSystem");
	qRegisterMetaType<ISClientsSqlModel*>("ISClientsSqlModel");
}
//-----------------------------------------------------------------------------
void ISAlpha::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISAlpha::InitializePlugin() const
{

}
//-----------------------------------------------------------------------------
