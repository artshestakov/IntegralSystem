#pragma once
#ifndef _ISALGORITHM_H_INCLUDED
#define _ISALGORITHM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
//Заснуть на Msec миллисекунд
#define ISSleep(MSec) std::this_thread::sleep_for(std::chrono::milliseconds(MSec))

//Подавление предупреждения неиспользуемого параметра
#define IS_UNUSED(x) (void)(x)

#ifdef WIN32
#define __CLASS__ ISAlgorithm::GetClassName(__FUNCTION__)
#else
#define __CLASS__ ISAlgorithm::GetClassName(__PRETTY_FUNCTION__)
#endif
//-----------------------------------------------------------------------------
#ifdef WIN32
#define CRITICAL_SECTION_INIT(CRITICAL_SECTION) InitializeCriticalSection(CRITICAL_SECTION)
#define CRITICAL_SECTION_LOCK(CRITICAL_SECTION) EnterCriticalSection(CRITICAL_SECTION)
#define CRITICAL_SECTION_UNLOCK(CRITICAL_SECTION) LeaveCriticalSection(CRITICAL_SECTION)
#define CRITICAL_SECTION_DESTROY(CRITICAL_SECTION) DeleteCriticalSection(CRITICAL_SECTION)
#define CURRENT_THREAD_ID GetCurrentThreadId
#define CURRENT_PID GetCurrentProcessId
#else
#define CRITICAL_SECTION_INIT(CRITICAL_SECTION) pthread_mutex_init(CRITICAL_SECTION, NULL)
#define CRITICAL_SECTION_LOCK(CRITICAL_SECTION) pthread_mutex_lock(CRITICAL_SECTION)
#define CRITICAL_SECTION_UNLOCK(CRITICAL_SECTION) pthread_mutex_unlock(CRITICAL_SECTION)
#define CRITICAL_SECTION_DESTROY(CRITICAL_SECTION) pthread_mutex_destroy(CRITICAL_SECTION)
#define CURRENT_THREAD_ID pthread_self
#define CURRENT_PID getpid
#endif
//-----------------------------------------------------------------------------
namespace ISAlgorithm
{
    //! Получить имя класса
    //! \return возвращает имя класса
    std::string GetClassName(char *FunctionName);

    //! Получить временную метку
    //! \return возвращает временную метку
    ISTimePoint GetTick();

    //! Получить разницу временных меток
    //! \param TickA временная метка
    //! \param TickB временная метка
    //! \return возвращает разницу между двумя временными метками
    ISUInt64 GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2);

    //! Получить описание последней ошибки
    std::string GetLastErrorS();

    //! Установить кодовую страницу
    //! \param CodePage номер кодовой страницы
    //! \param ErrorString ссылка на строку с ошибкой
    //! \return возвращает true в случае успешной установки, иначе - false
    bool ConsoleSetEncoding(unsigned int CodePage, std::string &ErrorString);

    //! Проверка существования папки
    //! \param DirPath путь к папке
    //! \return возвращает true в случае существования папки, иначе - false
    bool DirExist(const std::string &DirPath);

    //! Создание папки рекусивно
    //! \param DirPath путь к папке
    //! \return возвращает true в случае успешного создания папки, иначе - false
    bool DirCreate(const std::string &DirPath);

    //! Создание папки рекусивно
    //! \param DirPath путь к папке
    //! \param ErrorString ссылка на строку с ошибкой
    //! \return возвращает true в случае успешного создания папки, иначе - false
    bool DirCreate(const std::string &DirPath, std::string &ErrorString);

    //! Проверка существования файла
    //! \param FilePath путь к файлу
    //! \return возвращает true в случае существования файла, иначе - false
    bool FileExist(const std::string &FilePath);

    //! Удаление файла
    //! \param FilePath путь к файлу
    //! \return возвращает true в сулчае успешного удаления файла, иначе - false
    bool FileDelete(const std::string &FilePath);

    //! Удаление файла
    //! \param FilePath путь к файлу
    //! \param ErrorString ссылка на строку с ошибкой
    //! \return возвращает true в сулчае успешного удаления файла, иначе - false
    bool FileDelete(const std::string &FilePath, std::string &ErrorString);

    //! Получить путь к исполняемому файлу
    //! \return возвращает путь к испролняемому файлу
    std::string GetApplicationPath();

    //! Получить путь к папке приложения
    //! \return возвращает путь к папке приложения
    std::string GetApplicationDir();

    //! Получить имя приложения
    //! \return возвращает имя приложения
    std::string GetApplicationName();

    //! Получить имя компьютера
    //! \return возвращает имя компьютера
    std::string GetHostName();

    //! Получить имя пользователя
    //! \return возвращает имя текущего пользователя в ОС
    std::string GetUserName();

    //! Парсинг аргументов командной строки
    //! \param argc количество аргументов
    //! \param argv массив аргументов
    //! \return возвращает вектор аргументов
    ISVectorString ParseArgs(int argc, char **argv);

    //! Получить текущую дату и время
    //! \return возвращает структуру с текущей датой и временем
    ISDateTime GetCurrentDate();

    //! Разделить строку
    //! \param Separator разделитель
    //! \return возвращает вектор разделённых строк
    ISVectorString StringSplit(const std::string &String, char Separator);

    //! Проверка строки на число
    //! \param String строка
    //! \return возвращает true если строка является число, иначе - false
    bool StringIsNumber(const std::string &String);

    //! Приведение строки к нижнему регистру
    //! \param String строка
    void StringToLower(std::string &String);

    //! Приведение строки к верхнему регистру
    //! \param String строка
    void StringToUpper(std::string &String);

    //! Форматирование строки
    //! \param Format формат сообщения
    //! \param ... аргументы
    std::string StringF(const char *Format, ...);

    //! Генерация стандартного уникального идентификатора в формате b75ed238-411a-4f06-85ea-a2ecca37cfa8
    //! \return возвращает стандартный уникальный идентификатор
    std::string GenerateUuidStandart();

    //! Генерация уникального идентификатора в формате {B75ED238-411A-4F06-85EA-A2ECCA37CFA8}
    //! \return возвращает уникальный идентификатор
    std::string GenerateUuid();

    //! Генерация упрощённого уникального идентификатора в формате b75ed238411a4f0685eaa2ecca37cfa8
    //! \return возвращает упрощённый уникальный идентификатор
    std::string GenerateUuidLite();

    //Извлечь элемент из вектора по заданному индексу
    template <typename T> T VectorTakeAt(std::vector<T> &Vector, size_t Index)
    {
        T Value = Vector[Index];
        Vector.erase(Vector.begin() + Index);
        return Value;
    }

    //Извлечь первый элемент из вектора
    template <typename T> T VectorTakeFront(std::vector<T> &Vector)
    {
        return VectorTakeAt(Vector, 0);
    }

    //Извлечь последний элемент из вектора
    template <typename T> T VectorTakeBack(std::vector<T> &Vector)
    {
        return VectorTakeAt(Vector, Vector.size() - 1);
    }

    //Проверить наличие значения в векторе
    template <typename T> bool VectorContains(const std::vector<T> &Vector, T Value)
    {
        return std::find(Vector.begin(), Vector.end(), Value) != Vector.end();
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
}
//-----------------------------------------------------------------------------
#endif
