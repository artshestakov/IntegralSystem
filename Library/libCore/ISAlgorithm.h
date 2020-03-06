#pragma once
#ifndef _ISALGORITHM_H_INCLUDED
#define _ISALGORITHM_H_INCLUDED
//-----------------------------------------------------------------------------
//Поиск значения в векторе
template <typename T> bool VectorContains(const std::vector<T> &Vector, T Value)
{
	return std::find(Vector.begin(), Vector.end(), Value) != Vector.end();
}
//-----------------------------------------------------------------------------
//Извлечь элемент из вектора по индексу
template <typename T> T VectorTakeAt(std::vector<T> &Vector, size_t Index)
{
	T Value = Vector[Index];
	Vector.erase(Vector.begin() + Index);
	return Value;
}
//-----------------------------------------------------------------------------
//Извлечь первый элемент из вектора
template <typename T> T VectorTakeFront(std::vector<T> &Vector)
{
	return VectorTakeAt(Vector, 0);
}
//-----------------------------------------------------------------------------
//Извлечь последний элемент из вектора
template <typename T> T VectorTakeBack(std::vector<T> &Vector)
{
	return VectorTakeAt(Vector, Vector.size() - 1);
}
//-----------------------------------------------------------------------------
//Получить индекс элемента в векторе
template <typename T> size_t VectorIndexOf(const std::vector<T> &Vector, T Value)
{
	std::vector<T>::const_iterator Iterator = std::find(Vector.begin(), Vector.end(), Value);
	if (Iterator == Vector.end())
	{
		return size_t();
	}
	return std::distance(Vector.begin(), Iterator);
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
//Получить все знапчения словаря
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
//-----------------------------------------------------------------------------
#endif
