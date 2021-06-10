#include "ISArguments.h"
#include "ISConstants.h"
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
void ISArguments::AddFlag(const std::string &Name, const std::string &Description)
{
    ISArgumentItem ArgumentItem;
    ArgumentItem.Type = ArgumentType::Flag;
    ArgumentItem.Name = Name;
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
