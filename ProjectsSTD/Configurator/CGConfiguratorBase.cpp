#include "CGConfiguratorBase.h"
#include "ISLogger.h"
#include "ISConstants.h"
#include "ISAssert.h"
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
bool CGConfiguratorBase::Invoke(const std::string &FunctionName)
{
    auto It = MapFunction.find(FunctionName);
    IS_ASSERT(It != MapFunction.end(), ISAlgorithm::StringF("Not found function '%s'", FunctionName.c_str()));
    return (this->*It->second)();
}
//-----------------------------------------------------------------------------
bool CGConfiguratorBase::ExistFunction(const std::string &FunctionName)
{
    return MapFunction.find(FunctionName) != MapFunction.end();
}
//-----------------------------------------------------------------------------
void CGConfiguratorBase::Progress(const std::string &ObjectTypeName, size_t Step, size_t TotalStep, const std::string &Description)
{
    ISLOGGER_I(__CLASS__, "Progress \"%s\": %d of %d%s", ObjectTypeName, Step + 1, TotalStep, !Description.empty() ? ". " + Description : std::string());
}
//-----------------------------------------------------------------------------
void CGConfiguratorBase::RegisterFunction(const std::string &FunctionName, Function f)
{
    MapFunction.emplace(FunctionName, f);
}
//-----------------------------------------------------------------------------
