#include "ISArguments.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISArguments::ISArguments(int argc, char **argv)
    : ErrorString(STRING_NO_ERROR),
    ARGC(argc),
    ARGV(argv)
{

}
//-----------------------------------------------------------------------------
ISArguments::~ISArguments()
{

}
//-----------------------------------------------------------------------------
const std::string& ISArguments::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
std::string ISArguments::GetHelp() const
{
    std::string Result = "Arguments:\n";
    for (const ISArgumentItem &Item : Vector)
    {
        Result.append(ISAlgorithm::StringF("  %s,\t%-20s%-20s%\n",
            Item.SmallName.c_str(), Item.FullName.c_str(), Item.Description.c_str()));
    }
    ISAlgorithm::StringChop(Result, 1);
    return Result;
}
//-----------------------------------------------------------------------------
void ISArguments::AddFlag(const std::string &SmallName, const std::string &FullName, const std::string &Description)
{
    ISArgumentItem ArgumentItem;
    ArgumentItem.Type = ArgumentType::Flag;
    ArgumentItem.SmallName = SmallName;
    ArgumentItem.FullName = FullName;
    ArgumentItem.Description = Description;
    Vector.emplace_back(ArgumentItem);
}
//-----------------------------------------------------------------------------
bool ISArguments::IsExist(const char *Name)
{
    for (int i = 1; i < ARGC; ++i)
    {
        if (strcmp(ARGV[i], Name) == 0)
        {
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
bool ISArguments::IsEmpty()
{
    return ARGC == 1;
}
//-----------------------------------------------------------------------------
