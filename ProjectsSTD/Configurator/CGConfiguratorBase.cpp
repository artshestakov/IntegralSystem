#include "CGConfiguratorBase.h"
#include "ISLogger.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
CGConfiguratorBase::CGConfiguratorBase()
    : ErrorString(STRING_NO_ERROR)
{

}
//-----------------------------------------------------------------------------
CGConfiguratorBase::~CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
const std::string& CGConfiguratorBase::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
void CGConfiguratorBase::Progress(const std::string &ObjectTypeName, size_t Step, size_t TotalStep, const std::string &Description)
{
    ISLOGGER_I(__CLASS__, "Progress \"%s\": %d of %d%s", ObjectTypeName, Step + 1, TotalStep, !Description.empty() ? ". " + Description : std::string());
}
//-----------------------------------------------------------------------------
