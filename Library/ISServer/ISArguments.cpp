#include "ISArguments.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISArguments::ISArguments()
    : ErrorString(STRING_NO_ERROR)
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
        std::string FullName = Item.FullName;
        if (Item.Type == ArgumentType::Parameter)
        {
            FullName.append("=[VALUE]");
        }
        Result.append(ISAlgorithm::StringF("  %s,\t%-25s%-20s%\n",
            Item.SmallName.c_str(), FullName.c_str(), Item.Description.c_str()));
    }
    ISAlgorithm::StringChop(Result, 1);
    return Result;
}
//-----------------------------------------------------------------------------
void ISArguments::AddFlag(const std::string &SmallName, const std::string &FullName, const std::string &Description)
{
    Add(ArgumentType::Flag, SmallName, FullName, Description);
}
//-----------------------------------------------------------------------------
void ISArguments::AddParameter(const std::string &SmallName, const std::string &FullName, const std::string &Description)
{
    Add(ArgumentType::Parameter, SmallName, FullName, Description);
}
//-----------------------------------------------------------------------------
bool ISArguments::Parse(int argc, char **argv)
{
    size_t Pos = 0;
    for (int i = 1; i < argc; ++i)
    {
        //���������, ���� �� ������ '=' � ������� ���������
        std::string Argument(argv[i]), Value;
        Pos = Argument.find('=');
        if (Pos != NPOS) //������ ���� - ������� ��� �������� ����
        {
            Value = Argument.substr(Pos + 1, Argument.size() - Pos - 1);
            ISAlgorithm::StringChop(Argument, Argument.size() - Pos);

            //���� �������� ������ - ������
            if (Value.empty())
            {
                ErrorString = ISAlgorithm::StringF("Empty value parameter \"%s\"", Argument.c_str());
                return false;
            }
        }

        //���� ����� ����/�������� � �������
        //���� ��� ��� - ������
        ArgumentType Type = ArgumentType::Unknown;
        if (!ExistInVector(Argument, Type))
        {
            ErrorString = ISAlgorithm::StringF("Argument \"%s\" not support",
                Argument.c_str());
            return false;
        }

        //��������� �������� ���������� ��� ������
        //��������, ����� ������� ���� -t ����� ��������. ������ ���� �� ������
        if (Pos != NPOS && Type == ArgumentType::Flag)
        {
            ErrorString = ISAlgorithm::StringF("Illegal value \"%s\" from flag \"%s\"",
                Value.c_str(), Argument.c_str());
            return false;
        }

        //��������� �������� ��� ���������
        if (Pos != NPOS)
        {
            SetValue(Argument, Value);
        }

        //���������, ��� ����� �������� �������
        SetEnabled(Argument);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISArguments::IsExist(const std::string &Name)
{
    for (ISArgumentItem &Item : Vector)
    {
        if (Name == Item.SmallName || Name == Item.FullName)
        {
            return Item.Enabled;
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
std::string ISArguments::GetValue(const std::string &Name)
{
    std::string Value;
    for (const ISArgumentItem &Item : Vector)
    {
        if (Item.SmallName == Name || Item.FullName == Name)
        {
            Value = Item.Value;
            break;
        }
    }
    return Value;
}
//-----------------------------------------------------------------------------
void ISArguments::Add(ArgumentType Type, const std::string &SmallName, const std::string &FullName, const std::string &Description)
{
    ISArgumentItem ArgumentItem;
    ArgumentItem.Type = Type;
    ArgumentItem.SmallName = SmallName;
    ArgumentItem.FullName = FullName;
    ArgumentItem.Description = Description;
    Vector.emplace_back(ArgumentItem);
}
//-----------------------------------------------------------------------------
bool ISArguments::ExistInVector(const std::string &Name, ArgumentType &Type)
{
    for (const ISArgumentItem &Item : Vector)
    {
        if (Name == Item.SmallName || Name == Item.FullName)
        {
            Type = Item.Type;
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
void ISArguments::SetValue(const std::string &Name, const std::string &Value)
{
    for (ISArgumentItem &Item : Vector)
    {
        if (Item.Type == ArgumentType::Parameter && (Name == Item.SmallName || Name == Item.FullName))
        {
            Item.Value = Value;
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void ISArguments::SetEnabled(const std::string &Name)
{
    for (ISArgumentItem &Item : Vector)
    {
        if (Name == Item.SmallName || Name == Item.FullName)
        {
            Item.Enabled = true;
            break;
        }
    }
}
//-----------------------------------------------------------------------------
