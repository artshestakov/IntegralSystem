#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
ISObjectInterface::ISObjectInterface() : QObject()
{

}
//-----------------------------------------------------------------------------
ISObjectInterface::~ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
void ISObjectInterface::SetConfigurationName(const QString &configuration_name)
{
    ConfigurationName = configuration_name;
}
//-----------------------------------------------------------------------------
QIcon ISObjectInterface::GetIcon(const QString &IconName) const
{
    return QIcon(":_" + ConfigurationName + '/' + IconName + ".png");
}
//-----------------------------------------------------------------------------
