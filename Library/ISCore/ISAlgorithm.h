#pragma once
#ifndef _ISALGORITHM_H_INCLUDED
#define _ISALGORITHM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISSettingsDatabase.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
#define POINTER_DELETE(POINTER) \
if (POINTER) \
{ \
	delete(POINTER); \
	POINTER = nullptr; \
}

//Обработка дробного значения
#define DOUBLE_PREPARE(d) QString::number(d, 'f', SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA))

//Обработка денежного значения (кол-во знаков после запятой должно быть 2)
#define DOUBLE_PREPAREM(d) QString::number(d, 'f', 2)

//Заснуть на Msec миллисекунд
#define ISSleep(MSec) std::this_thread::sleep_for(std::chrono::milliseconds(MSec))

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
#define GET_PID GetCurrentProcessId
#else
#define CRITICAL_SECTION_INIT(CRITICAL_SECTION) pthread_mutex_init(CRITICAL_SECTION, NULL)
#define CRITICAL_SECTION_LOCK(CRITICAL_SECTION) pthread_mutex_lock(CRITICAL_SECTION)
#define CRITICAL_SECTION_UNLOCK(CRITICAL_SECTION) pthread_mutex_unlock(CRITICAL_SECTION)
#define CRITICAL_SECTION_DESTROY(CRITICAL_SECTION) pthread_mutex_destroy(CRITICAL_SECTION)
#define CURRENT_THREAD_ID pthread_self
#define GET_PID GetCurrentProcessId
#endif
//-----------------------------------------------------------------------------
namespace ISAlgorithm
{
	//! Вытаскивание имени файла из пути к нему
	//! \param FilePath путь к файлу
	//! \return возвращаем имя файла
	ISCORE_EXPORT std::string GetFileNameFromPath(const std::string &FilePath);

	//! Получить размер файла
	//! \param FilePath путь к файлу
	//! \return возвращает размер файла в байтах. В случае ошибки возвращается -1
    ISCORE_EXPORT qint64 GetFileSize(const std::string &FilePath);

	//! Получить размер файла
	//! \param FilePath путь к файлу
	//! \param ErrorString текстовое описание ошибки
	//! \return возвращает размер файла в байтах. В случае ошибки возвращается -1, а ErrorString заполняется описанием ошибки
    ISCORE_EXPORT qint64 GetFileSize(const std::string &FilePath, std::string &ErrorString);

	//! \Получить временную метку
	//! \return возвращает временную метку
	ISCORE_EXPORT ISTimePoint GetTick();

	//! \Получить разницу временных меток
	//! \param TickA временная метка
	//! \param TickB временная метка
	//! \return возвращает разницу между двумя временными метками
	ISCORE_EXPORT long long GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2);

	//! \Получить имя класса
	//! \param FunctionName в качестве этого параметра необходимо передавать макрос __FUNCTION__
	//! \return возвращает имя класса в случае успеха, иначе пустую строку
    ISCORE_EXPORT QString GetClassName(const char *FunctionName);

	//Поиск значения в векторе
    template <typename T> bool VectorContains(const std::vector<T> &Vector, T Value)
    {
        return std::find(Vector.begin(), Vector.end(), Value) != Vector.end();
    }

	//Извлечь значение из вектора
    template <typename T> bool VectorTake(std::vector<T> &Vector, T t)
	{
        typename std::vector<T>::const_iterator It = std::find(Vector.begin(), Vector.end(), t);
		bool Result = It != Vector.end();
		if (Result)
		{
			Vector.erase(It);
		}
		return Result;
	}

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

    //Получить индекс элемента в векторе
    template <typename T> size_t VectorIndexOf(const std::vector<T> &Vector, T Value)
    {
        typename std::vector<T>::const_iterator Iterator = std::find(Vector.begin(), Vector.end(), Value);
        if (Iterator == Vector.end())
        {
            return size_t();
        }
        return std::distance(Vector.begin(), Iterator);
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

    //Извлечь первое значение словаря
    template <typename TKey, typename TValue> TValue MapTakeFront(std::map<TKey, TValue> &Map)
    {
        typename std::map<TKey, TValue>::const_iterator It = Map.begin();
        if (It != Map.end())
        {
            TValue Value = It->second;
            Map.erase(It);
            return Value;
        }
        return TValue();
    }

	//Получить указатель на класс по его имени
	template <typename Class> Class CreatePointer(const QString &ClassName,
		QGenericArgument val0 = QGenericArgument(nullptr),
		QGenericArgument val1 = QGenericArgument(),
		QGenericArgument val2 = QGenericArgument(),
		QGenericArgument val3 = QGenericArgument(),
		QGenericArgument val4 = QGenericArgument(),
		QGenericArgument val5 = QGenericArgument(),
		QGenericArgument val6 = QGenericArgument(),
		QGenericArgument val7 = QGenericArgument(),
		QGenericArgument val8 = QGenericArgument(),
		QGenericArgument val9 = QGenericArgument())
	{
		Class Pointer = nullptr;
        int ObjectType = QMetaType::type((ClassName + '*').toLocal8Bit().constData());
		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		if (ObjectType && MetaObject)
		{
			Pointer = dynamic_cast<Class>(MetaObject->newInstance(val0, val1, val2, val3, val4, val5, val6, val7, val8, val9));
		}

		if (!Pointer)
		{
			IS_ASSERT(Pointer, "Not created pointer with class name \"" + ClassName + "\"");
		}
		return Pointer;
	}
}
//-----------------------------------------------------------------------------
#endif
