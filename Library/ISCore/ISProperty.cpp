#include "ISProperty.h"
//-----------------------------------------------------------------------------
ISProperty::ISProperty()
    : Uptime(0),
    ServerVersion(0),
    ChangeUser(false)
{

}
//-----------------------------------------------------------------------------
ISProperty::~ISProperty()
{

}
//-----------------------------------------------------------------------------
ISProperty& ISProperty::Instance()
{
    static ISProperty Property;
    return Property;
}
//-----------------------------------------------------------------------------
const uint64_t& ISProperty::GetUptime() const
{
    return Uptime;
}
//-----------------------------------------------------------------------------
void ISProperty::SetUptime(uint64_t uptime)
{
    Uptime = uptime;
}
//-----------------------------------------------------------------------------
const unsigned int& ISProperty::GetServerVersion() const
{
    return ServerVersion;
}
//-----------------------------------------------------------------------------
void ISProperty::SetServerVersion(unsigned int server_version)
{
    ServerVersion = server_version;
}
//-----------------------------------------------------------------------------
bool ISProperty::GetChangeUser() const
{
    return ChangeUser;
}
//-----------------------------------------------------------------------------
void ISProperty::SetChangeUser(bool change_version)
{
    ChangeUser = change_version;
}
//-----------------------------------------------------------------------------
const std::string& ISProperty::GetPathLastDirectory() const
{
    return PathLastDirectory;
}
//-----------------------------------------------------------------------------
void ISProperty::SetPathLastDirectory(const std::string &path_last_directory)
{
    PathLastDirectory = path_last_directory;
}
//-----------------------------------------------------------------------------
