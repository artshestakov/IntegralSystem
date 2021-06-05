#pragma once
#ifndef _ISALGORITHMOLD_H_INCLUDED
#define _ISALGORITHMOLD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISSettingsDatabase.h"
#include "ISTypedefsOld.h"
//-----------------------------------------------------------------------------

//Обработка дробного значения
#define DOUBLE_PREPAREN(d, n) QString::number(d, 'f', n)
#define DOUBLE_PREPARE(d) DOUBLE_PREPAREN(d, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA))

//Обработка денежного значения (кол-во знаков после запятой должно быть 2)
#define DOUBLE_PREPAREM(d) QString::number(d, 'f', 2)

//Обработать события из очереди событий
#define PROCESS_EVENTS qApp->processEvents

//Сгенерировать стандартный уникальный идентификатор
#define GENERATE_UUID_STANDART ISAlgorithmOld::GenerateUuidStandart

//Сгенерировать уникальный идентификатор
#define GENERATE_UUID ISAlgorithmOld::GenerateUuid

//Сгенерировать упрощенный уникальный идентификатор
#define GENERATE_UUID_LITE ISAlgorithmOld::GenerateUuidLite

//#ifdef WIN32
//#define __CLASS__ ISAlgorithmOld::GetClassName(__FUNCTION__)
//#else
//#define __CLASS__ ISAlgorithmOld::GetClassName(__PRETTY_FUNCTION__)
//#endif
//-----------------------------------------------------------------------------
namespace ISAlgorithmOld
{
    //! Получить последнюю ошибку на текущей системе
    //! \return возвращает строку с ошибкой
    QString GetLastErrorString();

    //! Сгенерировать соль
    //! \param Salt строка, в которую будет помещена сгенерированная соль
    //! \param ErrorString строка, в которую будет помещено описание ошибки
    //! \return возвращает true в случае успешное генерации, в противном случае - false
    bool GenerateSalt(QString &Salt, QString &ErrorString);

    //! Соление пароля
    //! \param HashPassword строка, которая должна содержать хэш логина и пароля (sha256(Login + Password))
    //! \param Salt строка, которая должна содержать соль (sha256(Login + Password))
    //! \return возвращает солёный пароль
    QString SaltPassword(const QString &HashPassword, const QString &Salt); //Посолить пароль

    //! Проверка пароля на соответствие требованиям сложности
    //! \param Password строка, содержащая пароль
    //! \return возвращает true в случае если пароль прошёл проверку, в противном случае - false
    bool PasswordVerification(const QString &Password);

    //! Подготовить номер телефона (удалить все кроме цифр)
    //! \param PhoneNumber строка, которая содержит номер телефона
    void FormatPhoneNumber(QString &PhoneNumber);

    //! Форматирование целого числа. Например, число 6985473 примет вид 6 985 473
    //! \Number целое число
    //! \return возвращает форматированное число
    QString FormatNumber(long long Number, char Separator = ' ');

    //! Форматирование дробного числа. Например, число 1024,965 примет вид 1 024,965
    //! \Number дробное число
    //! \return возвращает форматированное число
    QString FormatNumber(double Number, char Separator = ' ', unsigned int Precision = 0);

    //! Генерация стандартного уникального идентификатора в формате b75ed238-411a-4f06-85ea-a2ecca37cfa8
    //! \return возвращает стандартный уникальный идентификатор
    std::string GenerateUuidStandart();

    //! Генерация уникального идентификатора в формате {B75ED238-411A-4F06-85EA-A2ECCA37CFA8}
    //! \return возвращает уникальный идентификатор
    std::string GenerateUuid();

    //! Генерация упрощённого уникального идентификатора в формате B75ED238411A4F0685EAA2ECCA37CFA8
    //! \return возвращает упрощённый уникальный идентификатор
    std::string GenerateUuidLite();

    //Извлечь значение из вектора
    template <typename T> bool VectorTake(std::vector<T> &Vector, T t)
    {
        auto It = std::find(Vector.begin(), Vector.end(), t);
        bool Result = It != Vector.end();
        if (Result)
        {
            Vector.erase(It);
        }
        return Result;
    }

    //Удалить все элементы соответствующие значению
    template <typename T> void VectorRemoveAll(std::vector<T> &Vector, T Value)
    {
        for (size_t i = 0, c = Vector.size(); i < c; ++i)
        {
            if (Vector[i] == Value) //Если попалось значение - удаляем его
            {
                Vector.erase(Vector.begin() + i);
                --i, --c; //Обратно инкрементируем текущий индекс и размер (т.к. удалили один элемент)
            }
        }
    }

    //Получить все ключи словаря
    template <typename TKey, typename TValue> std::vector<TKey> ConvertMapToKeys(const std::map<TKey, TValue> &Map)
    {
        std::vector<TKey> Vector(Map.size());
        size_t Index = 0;
        for (const auto &MapItem : Map)
        {
            Vector[Index] = MapItem.first;
            ++Index;
        }
        return Vector;
    }

    //Получить все значения словаря
    template <typename TKey, typename TValue> std::vector<TValue> ConvertMapToValues(const std::map<TKey, TValue> &Map)
    {
        std::vector<TValue> Vector(Map.size());
        size_t Index = 0;
        for (const auto &MapItem : Map)
        {
            Vector[Index] = MapItem.second;
            ++Index;
        }
        return Vector;
    }
}
//-----------------------------------------------------------------------------
#endif
