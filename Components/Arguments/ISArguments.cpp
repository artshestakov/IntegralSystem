#include <string>
#include <vector>
#include "ISArguments.h"
//#include "ISConstants.h"
//#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISArguments::ISArguments()
    : ARGC(0)
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
    char Buffer[1024] = { 0 };
    std::string Result = "Arguments:\n";
    for (const ISArgumentItem &Item : Vector)
    {
        std::string FullName = Item.FullName;
        if (Item.Type == ArgumentType::Parameter)
        {
            FullName.append("=[VALUE]");
        }
        sprintf(Buffer, "  %s,\t%-25s%-20s\n",
            Item.SmallName.c_str(), FullName.c_str(), Item.Description.c_str());
        Result.append(Buffer);
    }
    Result.pop_back();
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
        //Проверяем, есть ли символ '=' в текущем аргументе
        std::string Argument(argv[i]), Value;
        Pos = Argument.find('=');
        if (Pos != std::string::npos) //Символ есть - считаем что значение есть
        {
            Value = Argument.substr(Pos + 1, Argument.size() - Pos - 1);
            Argument.erase(Argument.begin() + Pos, Argument.end());

            //Если значение пустое - ошибка
            if (Value.empty())
            {
                ErrorString = "Empty value parameter \"" + Argument + "\"";
                return false;
            }
        }

        //Ищем такой флаг/параметр в векторе
        //Если его нет - ошибка
        ArgumentType Type = ArgumentType::Unknown;
        if (!ExistInVector(Argument, Type))
        {
            ErrorString = "Argument \"" + Argument + "\" not support";
            return false;
        }

        //Запрещаем указание параметров для флагов
        //Ситуация, когда усовный флаг -t имеет значение. Такого быть не должно
        if (Pos != std::string::npos && Type == ArgumentType::Flag)
        {
            ErrorString = "Illegal value \"" + Value + "\" from flag \"" + Argument + "\"";
            return false;
        }

        //Установим значение для параметра
        if (Pos != std::string::npos)
        {
            SetValue(Argument, Value);
        }

        //Установим, что такой аргумент включён
        SetEnabled(Argument);

        ++ARGC; //Увеличим количество аргументов
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
    return ARGC == 0;
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
