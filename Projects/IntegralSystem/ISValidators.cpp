#include "ISValidators.h"
#include "ISTypedefsOld.h"
#include "ISAlgorithmOld.h"
#include "ISConstantsOld.h"
//-----------------------------------------------------------------------------
ISIntValidator::ISIntValidator(int Bottom, int Top, QObject *parent) : QIntValidator(Bottom, Top, parent)
{

}
//-----------------------------------------------------------------------------
ISIntValidator::ISIntValidator(QObject *parent) : ISIntValidator(0, 0, parent)
{

}
//-----------------------------------------------------------------------------
ISIntValidator::~ISIntValidator()
{

}
//-----------------------------------------------------------------------------
QIntValidator::State ISIntValidator::validate(QString &String, int &Pos) const
{
    if (String.isEmpty() && !Pos)
    {
        return ISIntValidator::Acceptable;
    }

    //Если первый символ минус, выставляем соответствующий флаг
    bool IsMinus = String.front() == '-';

    ISVectorInt VectorInt;
    for (int i = 0, c = String.size(); i < c; ++i) //Ищем индексы НЕ цифр
    {
        QChar Symbol = String[i];
        if (!Symbol.isDigit())
        {
            VectorInt.emplace_back(i);
        }
    }
    Pos -= (int)VectorInt.size(); //Отнимаем у позиции размер найденных НЕ цифр
    while (!VectorInt.empty()) //Удаляем не цифры
    {
        String.remove(ISAlgorithmOld::VectorTakeBack(VectorInt), 1);
    }

    //Проверяем "длину" числа: оно не должно быть длинее MAX_INTEGER_LEN
    String.chop(String.size() - MAX_INTEGER_LEN);

    if (IsMinus) //Если число отрицательное
    {
        if (String != '0') //И при этом это ноль - ничего не делаем (потому что ноль не может быть отрицательным), иначе - вставляем минус и инкрементируем позицию
        {
            String.insert(0, '-');
            ++Pos;
        }
    }

    //Если нижний или верхний пределы не нули - учитываем их
    if (bottom() != 0 || top() != 0)
    {
        bool Ok = true;
        int Value = String.toInt(&Ok); //Пытаемся перевести строку в число
        if (Ok) //Если все ок - продолжаем проверку
        {
            if (Value < bottom()) //Если введённое значение меньше нижнего предела - приравниваем значение к нижнему пределу
            {
                Value = bottom();
            }
            else if (Value > top()) //Если введённое значение больше верхнего предела - приравниваем значение к верхнему пределу
            {
                Value = top();
            }
            String.setNum(Value); //Устанавливаем значение в строку
        }
    }
    return ISIntValidator::Acceptable;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDoubleValidator::ISDoubleValidator(double Bottom, double Top, int Decimals, QObject *parent)
//Если количество знаков меньше нуля, или больше ограничения, или равно нулю - приравниваем его к ограничению
    : QDoubleValidator(Bottom, Top, Decimals < 0 || Decimals > MAX_DECIMAL_LEN || !Decimals ? MAX_DECIMAL_LEN : Decimals, parent)
{

}
//-----------------------------------------------------------------------------
ISDoubleValidator::ISDoubleValidator(int Decimals, QObject *parent) : ISDoubleValidator(0, 0, Decimals, parent)
{

}
//-----------------------------------------------------------------------------
ISDoubleValidator::~ISDoubleValidator()
{

}
//-----------------------------------------------------------------------------
QDoubleValidator::State ISDoubleValidator::validate(QString &String, int &Pos) const
{
    if (String.isEmpty() && !Pos) //Если поле было очищено
    {
        return QDoubleValidator::Acceptable;
    }

    //Если встретилась запятая - меняем на точку
    if (String.contains(SYMBOL_COMMA))
    {
        String.replace(SYMBOL_COMMA, SYMBOL_POINT);
    }

    //Если первый символ минус, выставляем соответствующий флаг
    bool IsMinus = String.front() == '-';

    ISVectorInt VectorInt;
    for (int i = 0, c = String.size(); i < c; ++i) //Ищем индексы НЕ цифр
    {
        QChar Symbol = String[i];
        if (Symbol.isDigit() || Symbol == SYMBOL_POINT) //Если текущий символ является цифрой и настроенным разделителем - пропускаем
        {
            continue;
        }
        VectorInt.emplace_back(i); //Иначе запоминаем индекс НЕ цифры и НЕ разделителя
    }
    Pos -= (int)VectorInt.size(); //Отнимаем у позиции размер найденных НЕ цифр

    //Удаляем не цифры
    while (!VectorInt.empty())
    {
        String.remove(ISAlgorithmOld::VectorTakeBack(VectorInt), 1);
    }

    //Ищем индекс первого попавшегося разделителя
    int SeparatorFirstIndex = String.indexOf(SYMBOL_POINT);
    if (SeparatorFirstIndex != -1) //Нашли разделитель - с этого индекса ищем и удаляем разделители (в итоге должен остаться только один разделитель)
    {
        while ((SeparatorFirstIndex = String.indexOf(SYMBOL_POINT, ++SeparatorFirstIndex)) != -1)
        {
            String.remove(SeparatorFirstIndex, 1);
        }
    }

    //Разбиваем строку на целое и дробное число
    QStringList StringList = String.split(SYMBOL_POINT);
    QString IntPart = StringList.front(); //Целая часть
    QString DoublePart = StringList.back(); //Дробная часть

    if (IntPart == DoublePart) //Если целая и дробная части равны - это целое число
    {
        IntPart.setNum(IntPart.toLongLong()); //Преобразовываем стандартным способом
        String.chop(String.size() - MAX_INTEGER_LEN); //Не даём целой части числа выйти за ограничение
    }
    else //Иначе дробное
    {
        IntPart.setNum(IntPart.toLongLong()); //Преобразовываем стандартным способом
        String.chop(String.size() - MAX_INTEGER_LEN); //Не даём целой части числа выйти за ограничение

        //Учитываем свойство Decimal и параллельно MAX_DECIMAL_LEN
        if (DoublePart.size() > decimals())
        {
            DoublePart.chop(DoublePart.size() - decimals());
        }

        //Собираем итоговое дробное число
        String = IntPart + SYMBOL_POINT + DoublePart;
    }

    if (IsMinus) //Если число отрицательное - вставляем минус и инкрементируем позицию
    {
        String.insert(0, '-');
        ++Pos;
    }

    //Если нижний или верхний пределы не нули - учитываем их
    if (bottom() != 0 || top() != 0)
    {
        bool Ok = true;
        double Value = String.toDouble(&Ok); //Пытаемся перевести строку в число
        if (Ok) //Если все ок - продолжаем проверку
        {
            if (Value < bottom()) //Если введённое значение меньше нижнего предела - приравниваем значение к нижнему пределу
            {
                String.setNum(bottom());
                return ISDoubleValidator::Acceptable;
            }
            else if (Value > top()) //Если введённое значение больше верхнего предела - приравниваем значение к верхнему пределу
            {
                String.setNum(top());
                return ISDoubleValidator::Acceptable;
            }

            if (String.back() != SYMBOL_POINT && String.back() != '0') //Вызываем setNum только когда последний символ строки не равен разделителю и не равен нулю
            {
                String.setNum(Value); //Устанавливаем значение в строку
            }
        }
    }
    return ISDoubleValidator::Acceptable;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISMoneyValidator::ISMoneyValidator(QObject *parent) : ISDoubleValidator(2, parent)
{

}
//-----------------------------------------------------------------------------
ISMoneyValidator::~ISMoneyValidator()
{

}
//-----------------------------------------------------------------------------
