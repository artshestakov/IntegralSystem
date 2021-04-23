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
    std::string GetClassName(const char *FunctionName);

    //! Получить временную метку
    //! \return возвращает временную метку
    ISTimePoint GetTick();

    //! Получить разницу временных меток
    //! \param TickA временная метка
    //! \param TickB временная метка
    //! \return возвращает разницу между двумя временными метками
    ISUInt64 GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2);

    //! Получить текущую метку времени в формате Unixtime
    //! \return возвращает текущую метку времени в формате Unixtime
    ISUInt64 GetCurrentUnixtime();

    //! Получить номер последней ошибки
    ISErrorNumber GetLastErrorN();

    //! Получить описание последней ошибки
    std::string GetLastErrorS();

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

    //! Получить список файлов в папке
    //! \param DirPath путь к папке
    //! \return возвращает список имён файлов. Если список пустой - произошла ошибка
    std::vector<ISFileInfo> DirFiles(const std::string &DirPath, ISNamespace::DirFileSorting SortType = ISNamespace::DirFileSorting::DoNotSort, ISNamespace::SortingOrder SortOrder = ISNamespace::SortingOrder::Ascending);

    //! Получить список файлов в папке
    //! \param DirPath путь к папке
    //! \param ErrorString ссылка на строку с ошибкой
    //! \return возвращает список имён файлов. Если список пустой - произошла ошибка
    std::vector<ISFileInfo> DirFiles(const std::string &DirPath, std::string &ErrorString, ISNamespace::DirFileSorting SortType = ISNamespace::DirFileSorting::DoNotSort, ISNamespace::SortingOrder SortOrder = ISNamespace::SortingOrder::Ascending);

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

    //! Получить N символов строки справа
    //! \param String строка
    //! \param N количество символов
    //! \return возвращает строку
    std::string StringRight(const std::string &String, size_t N);

    //! Удалить N символов в конце строки
    //! \param String строка
    //! \param N количество символов
    void StringChop(std::string &String, size_t N);

    //! Форматирование строки
    //! \param Format формат сообщения
    //! \param ... аргументы
    std::string StringF(const char *Format, ...);

    //! Формирование MD5-хэша из строки
    //! \param String строка, из которой будет вычислен MD5-хэш
    //! \return возвращает MD5-хэш. Если вернулась пустая строка - значит ошибка
    std::string StringToMD5(const std::string &String);

    //! Удалить указанный символ в конце
    //! \param String строка в которой будет происходить удаление
    //! \param Symbol символ который будет удаляться
    void RemoveLastSymbolLoop(std::string &String, char Symbol);

    //! Форматирование целого числа. Например, число 6985473 примет вид 6 985 473
    //! \Number целое число
    //! \return возвращает форматированное число
    std::string FormatNumber(long long Number, char Separator = ' ');

    //! Форматирование дробного числа. Например, число 1024,965 примет вид 1 024,965
    //! \Number дробное число
    //! \return возвращает форматированное число
    std::string FormatNumber(double Number, char Separator = ' ', unsigned int Precision = 0);

    //! Генерация стандартного уникального идентификатора в формате b75ed238-411a-4f06-85ea-a2ecca37cfa8
    //! \return возвращает стандартный уникальный идентификатор
    std::string GenerateUuidStandart();

    //! Генерация уникального идентификатора в формате {B75ED238-411A-4F06-85EA-A2ECCA37CFA8}
    //! \return возвращает уникальный идентификатор
    std::string GenerateUuid();

    //! Генерация упрощённого уникального идентификатора в формате b75ed238411a4f0685eaa2ecca37cfa8
    //! \return возвращает упрощённый уникальный идентификатор
    std::string GenerateUuidLite();

    //! Сгенерировать соль
    //! \param Salt строка, в которую будет помещена сгенерированная соль
    //! \param ErrorString строка, в которую будет помещено описание ошибки
    //! \return возвращает true в случае успешное генерации, в противном случае - false
    bool GenerateSalt(std::string &Salt, std::string &ErrorString);

    //! Соление пароля
    //! \param HashPassword строка, которая должна содержать хэш логина и пароля (sha256(Login + Password))
    //! \param Salt строка, которая должна содержать соль (sha256(Login + Password))
    //! \return возвращает солёный пароль
    std::string SaltPassword(const std::string &HashPassword, const std::string &Salt); //Посолить пароль

    //! Закодировать строку в base64
    //! \param String строку, которую нужно закодировать
    //! \return возвращает закодированную строку. В случае ошибки возвращается пустая строка
    std::string Base64Encode(const std::string &String, std::string &ErrorString);

    //! Закодировать массив в base64
    //! \param Data массив данных
    //! \param Size размер массив
    //! \return возвращает закодированную строку. В случае ошибки возвращается пустая строка
    std::string Base64Encode(unsigned char *Data, size_t Size, std::string &ErrorString);

    //! Прочитать файл
    //! \param FilePath путь к файлу
    //! \param Mode режим чтения
    //! \param FileSize размер файла
    //! \param ErrorString ссылка на строку с описанием ошибки
    //! \return Возвращает массив данных файла в случае отсутствия ошибки, иначе вернётся nullptr
    unsigned char* ReadFile(const char *FilePath, const char *Mode, long &FileSize);

    //! Прочитать файл
    //! \param FilePath путь к файлу
    //! \param Mode режим чтения
    //! \param FileSize размер файла
    //! \param ErrorString ссылка на строку с описанием ошибки
    //! \return Возвращает массив данных файла в случае отсутствия ошибки, иначе вернётся nullptr
    unsigned char* ReadFile(const char *FilePath, const char *Mode, long &FileSize, std::string &ErrorString);

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

    //Получить все ключи словаря
    template <typename TKey, typename TValue> std::vector<TKey> ConvertUnorderedMapToKeys(const std::unordered_map<TKey, TValue> &UnorderedMap)
    {
        std::vector<TKey> Vector(UnorderedMap.size());
        size_t Index = 0;
        for (const auto &MapItem : UnorderedMap)
        {
            Vector[Index] = MapItem.first;
            ++Index;
        }
        return Vector;
    }
}
//-----------------------------------------------------------------------------
#endif
