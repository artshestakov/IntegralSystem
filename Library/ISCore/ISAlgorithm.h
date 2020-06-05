#pragma once
#ifndef _ISALGORITHM_H_INCLUDED
#define _ISALGORITHM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
#define ISSleep(MSec) ::Sleep(MSec)
#define INIT_CRITICAL_SECTION(CRITICAL_SECTION) InitializeCriticalSection(CRITICAL_SECTION)
#define LOCK_CRITICAL_SECTION(CRITICAL_SECTION) EnterCriticalSection(CRITICAL_SECTION)
#define UNLOCK_CRITICAL_SECTION(CRITICAL_SECTION) LeaveCriticalSection(CRITICAL_SECTION)
#define DESTROY_CRITICAL_SECTION(CRITICAL_SECTION) DeleteCriticalSection(CRITICAL_SECTION)
#define GET_CURRENT_THREAD_ID GetCurrentThreadId
#else
#define ISSleep(MSec) usleep(MSec)
#define INIT_CRITICAL_SECTION(CRITICAL_SECTION) pthread_mutex_init(CRITICAL_SECTION, NULL)
#define LOCK_CRITICAL_SECTION(CRITICAL_SECTION) pthread_mutex_lock(CRITICAL_SECTION)
#define UNLOCK_CRITICAL_SECTION(CRITICAL_SECTION) pthread_mutex_unlock(CRITICAL_SECTION)
#define DESTROY_CRITICAL_SECTION(CRITICAL_SECTION) pthread_mutex_destroy(CRITICAL_SECTION)
#define GET_CURRENT_THREAD_ID pthread_self
#endif
//-----------------------------------------------------------------------------
namespace ISAlgorithm
{
    //Поиск значения в векторе
    template <typename T> bool VectorContains(const std::vector<T> &Vector, T Value)
    {
        return std::find(Vector.begin(), Vector.end(), Value) != Vector.end();
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
    template <typename TKey, typename TValue> std::vector<TValue> ConvertMapToKeys(const std::map<TKey, TValue> &Map)
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
		int ObjectType = QMetaType::type((ClassName + SYMBOL_STAR).toLocal8Bit().constData());
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

	//! Обрезание числа с плавающей запятой.
	//! \param Double число с плавающей запятой
	//! \param Precision количество цифр после запятой
	//! \return возращает обрезанное число с плаващей запятой
	template<typename T>
	T PrepareDouble(T Double, size_t Precision)
	{
		char Char[MAX_PATH];
		sprintf(Char, "%.*f", Precision, Double); //Конвертируем дробное число в строку.
		while (true) //Обрезаем возможные нули в конце
		{
			size_t Index = strlen(Char) - 1; //Индекс последнего символа
			if (Char[Index] == '0') //Если последний символ является нулем - обрезаем его
			{
				Char[Index] = '\0';
			}
			else if (Char[Index] == SYMBOL_POINT) //Если последний символ является точкой - обрезаем его и выходим
			{
				Char[Index] = '\0';
				break;
			}
			else //Иначе выходим из цикла
			{
				break;
			}
		}
		return atof(Char); //Преобразовываем строку обратно в число с плавающей запятой и возвращаем
	}
}
//-----------------------------------------------------------------------------
#endif
