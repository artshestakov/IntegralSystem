#include "ISFavorites.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISFavorites::ISFavorites()
{

}
//-----------------------------------------------------------------------------
ISFavorites::~ISFavorites()
{

}
//-----------------------------------------------------------------------------
ISFavorites& ISFavorites::Instance()
{
    static ISFavorites Favorites;
    return Favorites;
}
//-----------------------------------------------------------------------------
QVariantMap ISFavorites::GetFavorites()
{
    QVariantMap VariantMap;
    for (const auto &MapItem : Favorites)
    {
        QVariantList ObjectList;
        for (unsigned int ObjectID : MapItem.second)
        {
            ObjectList.append(ObjectID);
        }
        VariantMap[MapItem.first] = ObjectList;
    }
    return VariantMap;
}
//-----------------------------------------------------------------------------
void ISFavorites::Initialize(const QVariantMap &VariantMap)
{
    for (const auto &MapItem : VariantMap.toStdMap())
    {
        QVariantList Objects = MapItem.second.toList();
        size_t Size = (size_t)Objects.size();
        ISVectorUInt VectorInt(Size);
        for (size_t i = 0; i < Size; ++i)
        {
            VectorInt[i] = Objects[(int)i].toUInt();
        }
        Favorites[MapItem.first] = VectorInt;
    }
}
//-----------------------------------------------------------------------------
void ISFavorites::Add(const QString &TableName, unsigned int ObjectID)
{
    Favorites[TableName].emplace_back(ObjectID);
}
//-----------------------------------------------------------------------------
void ISFavorites::Delete(const QString &TableName, unsigned int ObjectID)
{
    ISAlgorithm::VectorRemoveAll(Favorites[TableName], ObjectID);
    if (Favorites[TableName].empty())
    {
        Favorites.erase(TableName);
    }
}
//-----------------------------------------------------------------------------
void ISFavorites::DeleteAll()
{
    Favorites.clear();
}
//-----------------------------------------------------------------------------
bool ISFavorites::Exist(const QString &TableName, unsigned int ObjectID)
{
    auto It = Favorites.find(TableName);
    if (It != Favorites.end())
    {
        return ISAlgorithm::VectorContains(It->second, ObjectID);
    }
    return false;
}
//-----------------------------------------------------------------------------
