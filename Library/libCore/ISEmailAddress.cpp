#include "ISEmailAddress.h"
//-----------------------------------------------------------------------------
ISEmailAddress::ISEmailAddress()
{

}
//-----------------------------------------------------------------------------
ISEmailAddress::ISEmailAddress(const QString &address, const QString &name)
{
    Address = address;
    Name = name;
}
//-----------------------------------------------------------------------------
ISEmailAddress::~ISEmailAddress()
{

}
//-----------------------------------------------------------------------------
void ISEmailAddress::setName(const QString &name)
{
    Name = name;
}
//-----------------------------------------------------------------------------
void ISEmailAddress::setAddress(const QString &address)
{
    Address = address;
}
//-----------------------------------------------------------------------------
const QString& ISEmailAddress::getName() const
{
    return Name;
}
//-----------------------------------------------------------------------------
const QString& ISEmailAddress::getAddress() const
{
    return Address;
}
//-----------------------------------------------------------------------------
